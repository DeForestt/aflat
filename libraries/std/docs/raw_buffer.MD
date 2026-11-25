# raw_buffer: storage for collection implementers

```aflat
import raw_buffer from "Collections/raw_buffer";
```

`raw_buffer::<T>` is an implementation tool for data structures and collections.
It centralizes allocation, capacity bounds checks, slot addressing, byte movement,
and selected destruction operations so collection authors need less unsafe code.
**Direct use in application code is a code smell.** Prefer `vector`,
`unordered_map`, or an owning collection with an API suited to the application.
Keep the buffer private behind that collection's interface.

## Ownership belongs to the enclosing collection

The buffer owns its backing allocation and lock. It **does not enforce element
ownership**, track initialized slots, or maintain a logical length. Its `unique`
declaration does not make the contents ownership-safe. Bounds checks establish
only that an index is within allocated capacity; an in-bounds slot may contain
uninitialized bytes, a moved value, or an already-destroyed object.

The higher-level collection must:

- Track length or occupancy and which slots hold live, initialized values.
- Initialize slots before reading, taking, replacing, or destroying their values.
- Define insertion, copy, move, replacement, removal, and aliasing policies.
  Use `write` for vacant unique slots and `replace` for live unique slots.
- Mark a slot vacant after a successful `take` or `destroy`, and never access its
  old value again. Neither operation clears bytes or records this state.
- After relocation or transfer, record the destination as live and source-only
  slots as vacant. Do not destroy the stale bytes left in those vacant slots.
- Destroy each remaining live unique element exactly once before dropping the
  buffer. Handle other resources according to the collection's ownership policy.
  The buffer destructor frees storage and the lock; it does **not** walk elements.
- Keep loans and referenced resources alive, prevent mutation while a conflicting
  loan exists, and coordinate compound operations and destruction across threads.
- Handle errors before updating length/occupancy. Unique arguments to `write`
  and `replace` are consumed even when the index is invalid; rejected arguments
  are destroyed rather than returned to the caller.

Overwriting a live unique slot with `write` can leak its resources. Reading an
uninitialized slot, taking the same unique element twice, or destroying an element
twice can cause invalid memory access or double destruction. These mistakes are
not converted into `Err` results.

## Operations

Single-slot operations reject negative indices and indices at or beyond capacity.
Range operations allow an index equal to capacity only for a zero-length range;
even empty ranges reject negative or greater-than-capacity indices. Negative
counts and ranges exceeding either buffer's capacity are rejected without adding
unchecked indices and counts. Bounds failures leave storage unchanged.
These operations return a local `result`; successful mutations return `Ok(true)`.

| Operation | Behavior and caller obligations |
| --- | --- |
| `raw_buffer::<T>(capacity)` | Allocates uninitialized storage. Zero is allowed. Negative capacity, a byte size exceeding the signed `int` limit, or allocation failure panics. |
| `capacity()` | Returns allocated slots, not live-element count. |
| `grow(newCapacity)` | Rejects shrinking, byte-size overflow, or allocation failure. Equal capacity is a successful no-op. Growth copies existing storage to a new allocation without invoking element constructors, copy hooks, or destructors. New slots are uninitialized. Failure leaves existing storage intact. |
| `read(index)` | Returns a primitive value, a class copy when `T` has `__copy__`, or `result::<&T>` borrowing the slot when it does not. Requires a live slot. |
| `borrow(index)` | For class elements, returns `result::<&T>` even when `T` has `__copy__`. Never copies, allocates an element, or transfers ownership. Requires a live slot; the caller controls the loan's lifetime. Use `read` for primitive values. |
| `write(index, value)` | Writes without destroying old contents. Unique classes are transferred into the slot and their former outer allocation is freed. Non-unique classes are shallow byte-copied, without invoking `__copy__` or acquiring ownership of nested resources. |
| `replace(index, value)` | For unique classes, destroys the old element in place before transferring the replacement. Other types behave like `write`. Requires a live unique slot. |
| `take(index)` | For unique classes, allocates a separate object and moves bytes into it without invoking `__copy__` or destroying the source. Allocation failure leaves the slot live. Primitives are read by value. Non-unique classes use the result type's copy/reference behavior; do not assume an independent owning value. The collection must record removal after success. |
| `destroy(index)` | Calls the in-place destructor for a unique class, without freeing the slot. For primitives and non-unique classes this only checks bounds. The collection must record removal. |
| `relocate(source, destination, count)` | Moves a range within the same buffer in the correct direction for overlap. Source slots must be live; destination slots outside that source range must be vacant. After success, destination slots are live and source-only slots are vacant. Equal source/destination or a valid empty range is a no-op. |
| `swap(left, right)` | Exchanges two live slots using byte-sized local scratch storage. Equal valid indices are a no-op. Does not copy elements through `__copy__`, destroy them, or change which slots are live. |
| `transfer_to(destination, sourceIndex, destinationIndex, count)` | Moves a live source range into vacant slots of another buffer of the same element type. Does not consume either buffer or grow capacity. Source slots become logically vacant after success. Passing the same buffer delegates to overlap-aware `relocate`. |
| `del()` | Frees backing storage and the lock only. The collection must already have cleaned up live elements and ended all access. |

### Collection policy versus movement mechanics

`relocate`, `swap`, and `transfer_to` move bytes without invoking constructors,
copy hooks, transfer hooks, or destructors. Successful calls allocate no heap
storage, including scratch buffers and result wrappers. Invalid calls may allocate
an `Error`. None of these operations validates element initialization or ownership,
and moved source bytes are not cleared. These are movement primitives, not copying
APIs: do not continue treating both source and destination as owners.

The enclosing collection owns insertion and removal. For example, a vector
inserting at `index` validates against its live length, grows if necessary,
calls `relocate(index, index + 1, length - index)`, writes into the resulting
vacant slot, and updates length. Removing an element first destroys or takes it,
then shifts the trailing range left and decrements length. These sequences require
the collection's synchronization and error handling. The buffer neither tracks a
dense live prefix nor assumes that one exists.

For a consuming vector append, the collection can reserve destination capacity,
call `source.transfer_to(destination, 0, destinationLength, sourceLength)`, and
update both lengths only after success. It can then drop the empty source buffer.
The transfer itself neither frees that buffer nor destroys its former elements.

### Address and non-unique writes use a source address

The `adr` and non-unique class overloads accept `any` and copy `T` bytes **from**
that value. They cannot validate the source pointer, its size, its type, or its
lifetime. For an `adr` element, pass the address of the pointer variable:

```aflat
// Inside a collection implementation; handle the result before marking slot 0 live.
const adr pointer = "payload";
const local result::<bool> written = storage.write(0, ?pointer);
```

Passing `pointer` instead would copy bytes from the pointed-to payload. Other
primitive overloads take their values directly. Shallow copies of non-unique
classes do not establish ownership of embedded pointers or other resources;
the enclosing collection must supply a compatible policy or avoid those types.

## Lifetime, relocation, and synchronization limits

Growth relocates raw bytes and frees the old allocation. Every loan into that
allocation becomes invalid. Replacement, destruction, slot reuse, taking an
element, and buffer destruction also end the old element's usable lifetime.
Relocation, transfer, and swaps invalidate loans into affected slots; end those
loans before moving elements and obtain fresh ones afterward. A successful
`borrow` releases the internal lock before returning and does not pin its slot.
Do not retain a loan across these operations. In particular, a non-unique
`take` must not be treated as a lifetime-independent extraction unless its copy
behavior and resource policy make that valid.

Stored types must tolerate byte relocation. Self-referential objects, pointers
into their own inline storage, and types requiring a relocation hook are not
supported by the raw growth/move operations. `take` of a unique object allocates;
it is not an allocation-free removal primitive.

The internal lock serializes individual storage operations. It does not protect
a returned loan after the method returns, make a sequence such as
check-length/write/increment-length atomic, or synchronize buffer destruction.
The collection must protect those larger invariants. Methods marked `safe`
still rely on the live-slot and lifetime preconditions above.

Transfers between distinct buffers hold both internal locks in a stable order
based on the addresses of their capacity fields. Opposing transfers therefore
use the same lock order, including for empty buffers; growth cannot change that
order. Self-transfers use one buffer lock. This does not establish a lock order
for enclosing collections: callers must still coordinate their own locks, avoid
conflicting callback lock orders, and keep both buffers alive throughout the call.

The buffer has no automatic shrinking, iteration, logical removal bookkeeping,
element cleanup on drop, or validation of aliases. Its purpose is to concentrate
unsafe implementation work, not to provide an application collection's safety
guarantees.

## Validation

From the repository root, after building the compiler and standard libraries:

```sh
python3 test/runtime/raw_buffer/run.py
```

The suites import the standard library module and exercise primitive and class
overloads, growth, invalid indices, copying, loans, transfers, and destruction.
Movement tests cover overlapping and disjoint ranges, invalid and empty ranges,
allocation counts, copy/destructor counts, and opposing concurrent transfers.
