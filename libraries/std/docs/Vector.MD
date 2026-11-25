# vector storage and ownership

```aflat
import vector from "Collections/Vector";
```

`vector::<T>` keeps a contiguous live prefix in a private `raw_buffer::<T>`.
Vector owns the live count, growth policy, iteration state, synchronization of
compound operations, and cleanup of unique elements. The buffer implements
allocation, capacity checks, borrowing, replacement, relocation, and swapping.

The existing application API remains on vector: `push_back`, `get`, `set`,
`insert`, `drop`, `pop_front`, `pop_back`, `extend`, `stitch`, and `sort`.
Applications should use vector rather than managing raw-buffer slot lifetimes.

## Growth and failures

The default initial capacity is one. Insertion grows geometrically when full,
with checked capacity arithmetic. Capacity and live count are updated only after
the corresponding operation succeeds. New storage does not create live elements.

`get` and removal/accessor options use live count as their bound, not capacity.
`set`, `insert`, and `drop` reject negative and out-of-range indices. `extend`
requires a strictly larger capacity and returns an error if growth fails or the
requested byte size cannot be represented. `insert` also reports growth errors.
`push_back` and `stitch` retain their fluent return types and panic on growth
failure. Consuming element arguments are consumed even when insertion or
replacement is rejected.

## Copying, borrowing, and cleanup

Primitive reads return values. Class reads return an independent `__copy__`
result when that hook exists; otherwise they return a loan into vector storage.
Options and temporary lock guards use local storage.

Unique elements are transferred into the vector. Replacement and dropping an
element run its destructor once. On vector destruction, vector destroys its live
unique elements before releasing the raw buffer. Shifts and swaps relocate bytes
without invoking copy hooks or destructors. Types must tolerate byte relocation.

Non-unique class insertion retains the existing shallow-copy policy. Vector does
not automatically destroy these elements or acquire ownership of their nested
resources. Higher-level owners such as map buckets remain responsible for their
explicit cleanup policies.

Borrowed values must not outlive the vector or operations that relocate, replace,
destroy, or reorder their slots. A method's lock is released before a returned
loan is used. The caller must provide any synchronization needed for that use.
`get_unlocked` skips the vector lock for callers that already coordinate access;
the raw-buffer access still takes its own internal lock.

## Stitching

`destination.stitch(source)` preserves the source. Primitive values and
non-unique classes retain their value/shallow-copy behavior. **Unique classes
with `__copy__` are copied**, so the vectors do not own the same resources.
Borrowed stitching is unavailable for move-only unique elements; use
`destination.stitch($source)` instead.

Consuming stitching transfers the source range into reserved destination storage,
marks the source empty, and releases the consumed vector. It does not allocate a
temporary element buffer or invoke element copy/destructor hooks during transfer.
Borrowed self-stitching appends the original live prefix once. Consuming a vector
into itself is rejected. Two-vector operations acquire vector locks in a stable
order; callers must still avoid conflicting external lock orders.

## Sorting and allocations

Sorting borrows class elements for comparison and swaps their slots through the
buffer, without allocating scratch storage or calling `__copy__`. Primitive
comparators receive values. Address-element comparators receive addresses
containing pointer values, valid only during the callback. Comparators must only
observe their arguments: do not retain those addresses or mutate the vector.

Within existing capacity, primitive appends, lookups, iteration, search, and pops
do not allocate. Copy hooks and owned-value extraction may allocate. Public
mutation methods returning `result::<bool>` retain their heap-result API.
Creating a vector now also creates a raw-buffer wrapper and its lock; storage
operations take that lock beneath the vector's operation lock. This migration
centralizes memory handling; it is not a claim that lock overhead has decreased.

## Validation

Rebuild all standard libraries after changing vector's layout. Precompiled modules
can contain generic vector specializations, so mixing old and new assemblies can
mix incompatible layouts. On constrained hosts, use serial builds:

```sh
bash ./rebuild-libs.sh -j 1
python3 test/runtime/vector/run.py
```

The runtime suites exercise allocation counts, ownership, insertion/removal,
growth failure, stitching, address values, map use, and heap use. The runner builds
one suite at a time with a 2 GiB address-space limit, CPU limit, and timeout.
