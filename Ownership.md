# Ownership, borrowing, and local storage

Ownership answers **who cleans up an object**. Storage answers **where that
object lives**. AFlat tracks both: a stack object can be owned, and a pointer to
a heap object can be borrowed. This guide describes the current compiler and
the typed `option`/`result` APIs.

## Defaults and cleanup

Classes and unions are unique by default. `class Resource` and
`unique class Resource` use the same ownership policy. An owner is responsible
for destroying the value unless it transfers that responsibility elsewhere.
An explicit `__copy__` can provide a separate owned copy; `unique` does not
prohibit a type from implementing copying.

At scope exit, an owned unique value runs `del`, including generated cleanup
for owned fields or the active union payload. Heap storage is then freed;
stack storage is not passed to `af_free`. Sold or returned values are excluded
from the original owner's cleanup. `delete value;` requests early destruction.

`shared class` and `shared union` opt out of the unique default. Shared classes
use the `endScope` lifecycle hook, when supplied, instead of the unique cleanup
policy. The modifier does not create reference counting, copy an object, or
provide thread synchronization. Types that need those behaviors must implement
them, for example through library lifecycle hooks.

Primitive values such as integers and booleans copy by value. `adr` is also a
primitive: copying an address does not transfer ownership of its allocation.
Code using raw allocations must release them or place them in an appropriate
owner such as `Memory.owned::<adr>`.

## Heap and stack construction

```aflat
.needs <std>

class Item {
    int value = value;
    fn init(const int value) -> Self { return my; };
};

fn makeHeap() -> Item { return new Item(10); };
fn makeLocal() -> local Item { return Item(20); };

fn main() -> int {
    const let heap = makeHeap();
    const local let local = makeLocal();
    const let alsoLocal = Item(30);
    return heap.value + local.value + alsoLocal.value - 60;
};
```

`new Item(...)` allocates on the heap. `Item(...)` constructs in stack storage;
`dynamic` classes require heap construction. A `local` binding requires a
stack value and can select a local-return overload. It does not turn an
existing heap allocation into a stack allocation.

Stack objects still own their resources and run cleanup. A local object may
own heap-backed fields; keeping the outer object on the stack does not make
its payloads allocation-free.

## Parameters and explicit transfers

| Declaration | Meaning for a non-primitive argument |
| --- | --- |
| `const T value` | Borrow the argument; the caller keeps ownership. |
| `immutable T value` | Borrow with read-only access; only safe methods are available. |
| `const T &&value` | Consume a heap-owned value. |
| `const local T value` | Borrow a stack-local value. |
| `const local T &&value` | Consume a stack-local value's cleanup responsibility. |

Pass an owned temporary directly to a consuming parameter. Use `$value` to
transfer a named owner:

```aflat
fn consume(const Item &&value) -> int { return value.value; };
fn consumeLocal(const local Item &&value) -> int { return value.value; };

// Inside a function, with the Item definition above:
const let first = new Item(1);
const int a = consume($first);
const int b = consume(new Item(2));
const let second = Item(3);
const int c = consumeLocal($second);
```

After a transfer, the old binding is sold and cannot be used as a live owner.
A regular borrowed parameter cannot be sold. A stack value cannot be passed
to a heap-consuming parameter just by adding `$`; ownership transfer does not
change its storage.

An ordinary assignment or field read does not implicitly sell the source.
Use an explicit transfer for a new owner, or a type's copy API to create an
independent value. In particular, an owning field cannot accept a borrowed
object as though it owned it.

`const` restricts rebinding but permits calls to ordinary methods that mutate
the object. `immutable` also restricts object access to `safe` methods. Neither
modifier chooses heap versus stack storage or replaces the ownership rules.

## Returning values

| Return declaration | Behavior |
| --- | --- |
| `-> T` | Return an owned heap object when `T` is an owning object type. |
| `-> local T` | Transfer a stack object into storage supplied by the caller. |
| `-> loan T` | Return a borrowed view; no ownership transfers. |
| `-> loan immutable T` | Return a borrowed view requiring immutable access. |

A named owner can be returned with `return value;`; the return path transfers
cleanup responsibility. This is different from passing a named owner to an
`&&` parameter, which requires `$value`.

Local returns do not return a pointer into an expired callee frame. The caller
provides the destination and becomes responsible for cleanup. A local-return
function can forward another local-return expression. A stack object cannot
satisfy an ordinary owned return; the compiler reports:

```text
cannot return stack storage as an owned pointer; use a local return
```

Borrowed returns depend on the referent's lifetime. Returning a loan to an
object owned only by the current function is rejected where the compiler
tracks that provenance. Wrapping that loan in an option does not extend it.

## Methods and fields

Ordinary methods borrow `my`. A `sink fn` consumes its receiver. A named
receiver must be explicitly sold, for example `$owner.release()`. An owned
temporary can select a sink overload directly; if only a borrowing overload
matches, it can borrow the temporary for the call. An explicit receiver sale
requires a compatible sink overload.

Reading `object.field` borrows a non-primitive field. Owning the parent does
not make that field read an independent owner. `$object.field` is rejected
outside the owning object's implementation. An explicit method may transfer
`$my.field`; that method must reset the field or otherwise maintain valid
instance state so cleanup cannot destroy the transferred value again.

`local T field` embeds storage in its parent. Its cleanup follows the parent,
and its value cannot be sold independently as a heap pointer. Move the parent
or make a separate copy instead.

`loan T field` explicitly stores a non-owning reference. Its referent is not
destroyed with the parent. The programmer must keep it alive; this is not a
general-purpose lifetime-safe replacement for owning fields.

## Options, results, and borrowed payloads

The lowercase `Utils/option` and `Utils/result` modules define unique unions.
Their container ownership is separate from their payload ownership:

| Type | Payload policy |
| --- | --- |
| `option::<int>` | Copy a primitive value. |
| `option::<Item>` | Own an Item payload. |
| `option::<&Item>` | Borrow an existing Item without copying or destroying it. |
| `result::<Item>` | Own an Item on success or an Error on failure. |
| `result::<&Item>` | Borrow Item on success; the error branch still owns Error. |

`&T` in a generic argument describes a loaned payload. It is distinct from
`?value` (taking the address of a variable) and `T&` reference parameters.
The compiler propagates loan provenance and rejects returning a loaned payload
whose referent belongs to the current function. Keep the original owner alive
and avoid operations that invalidate its storage while using the loan.

Construct owning payloads from an owned temporary, `$value`, or a supported
`__copy__`. A plain borrowed object without a copy hook cannot become an owned
payload merely by putting it in `Some` or `Ok`.

`Some`, `None`, `optionWrapper`, `accept`, `reject`, and `resultWrapper` have
heap- and local-return overloads. A local binding, local-return context, or
local parameter can select the local overload:

```aflat
import option from "Utils/option";
import {Some, None} from "Utils/option" under opt;

fn present() -> local option::<int> { return opt.Some::<int>(0); };
fn absent() -> local option::<int> { return opt.None::<int>(); };
```

Local wrapper construction avoids a heap allocation for the wrapper. Payload
construction and copying can still allocate. If both return-storage overloads
exist, ordinary contexts choose the heap version. A function that only has a
local-return version remains callable without a `local` binding.

### Optional parameters and return sugar

```aflat
fn readOptional(?const int value) -> int {
    match value { Some(n) => return n, None() => return -1 };
};

fn heapOptional(const int value) -> int? { return value; };
fn localOptional(const int value) -> local int? { return value; };
```

With the option module and wrapper helper available, `?T` parameters accept a
raw `T` from the caller. An omitted argument is passed as zero/NULL. At function
entry, the compiler always uses a **local option wrapper**, even when the
parameter does not say `local`. That choice affects the wrapper, not the
incoming payload's ownership or storage requirements. For example,
`?const Item &&value` still consumes a heap-owned Item, while its option
container is local.

The implicit wrapper probes for a zero/NULL representation: `0` is `None`, not
`Some(0)`. Use an explicit option parameter and `Some(0)` when zero is a valid
present value. The same null-probing rule applies to bare values returned
through `T?`; returning an already constructed option preserves its variant.
`return;` in an optional-return function produces `None`.

`-> T?` returns a heap option; `-> local T?` returns a local option. Therefore,
returning the generated parameter wrapper directly from an ordinary `T?`
function is rejected. Use a compatible local return or construct a separate
owned result. Ordinary explicit `option::<T>` parameters are borrowed or
consumed according to their declared modifiers; they are not implicitly wrapped.

`-> T!` similarly abbreviates `result::<T>`, and `-> local T!` selects local
result storage. `!` means a result type, not an ownership annotation. The postfix
bubble operator `expression!` unwraps success or propagates an owned error.

### Matching and extraction

Matching a named union normally borrows its non-primitive payload. Use a
consuming match and `&&` payload binding when taking ownership out:

```aflat
fn take(const option::<Item> &&wrapped) -> Item {
    match $wrapped {
        Some(&&item) => return $item,
        None() => panic("missing item")
    };
};
```

For non-primitive payloads, `unwrap` and `expect` provide borrowing and sink
overloads. `wrapped.unwrap()` borrows; `$wrapped.unwrap()` consumes an owning
wrapper to extract its owned payload. A wrapper containing `&T` cannot turn
that loan into ownership. Primitive extraction copies the value.

## Collections and borrowed elements

Vector `get`, `front`, `back`, `next`, `findFirst`, and pop methods return local
options. Primitive lookups copy; object lookups use `__copy__` when available,
otherwise they return `option::<&T>`. Popping removes an element and returns an
owning payload, which may require a heap allocation even though its option
wrapper is local.

For a Student without `__copy__`, `roster.findFirst(...)` borrows the existing
Student. Mutating it through a permitted method updates that Student in place.
Do not `$student` or pass it back to `roster.set` as an owner. For a copyable
element, lookup returns an independent copy and replacement is a separate
operation.

Unordered-map lookup follows the same copy-or-borrow payload distinction, but
its current option-returning APIs use heap wrappers. Do not infer a wrapper's
storage from its name; check the method's return declaration.

Container locks protect the operation, not the lifetime of an escaped borrow.
Do not resize, remove, destroy, or concurrently invalidate an element while
holding a view into its storage.

## Limits of the checks

Ownership checks catch invalid sales, owned-return mismatches, and tracked
local-loan escapes. They do not provide a complete aliasing or lifetime proof
for raw addresses, casts, foreign code, unsafe loan fields, or concurrent
container mutation. Async work also does not automatically extend a captured
borrow's lifetime; see [the async guide](Async.md#ownership-lifetimes-and-shared-state).

For syntax see [Docs.md](Docs.md); for a worked introduction see
[First Steps](first-steps.MD#aflat-ownership-model).
