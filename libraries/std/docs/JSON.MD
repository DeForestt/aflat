# JSON

The `JSON` module provides synchronous and native-task APIs for parsing and
serializing JSON values.

## Parsing

Put these imports at module scope and the example statements inside an
`async fn`. Include `.needs <std>` and `.needs <Async.gs>` in the module.

```aflat
import string from "String";
import JSON from "JSON";
import {parse, parseAsync} from "JSON" under json;

const string source = "{\"ready\":true}";

// Synchronous: result<JSON>
const let immediate = json.parse(source);

// Asynchronous constructor: task<result<JSON>>
const let pending = json.parseAsync(source);
const let parsed = await pending;
```

From synchronous code, use `run json.parseAsync(source)` to schedule the task
and wait for its `result<JSON>` value.

## Serialization

Every JSON value supports compact and indented asynchronous serialization:

```aflat
const string compact = await value.stringifyAsync();
const string pretty = await value.stringifyPrettyAsync(1);
```

The module-level helper accepts an optional indentation value:

```aflat
import {stringifyAsync} from "JSON" under json;

const string compact = await json.stringifyAsync(value);
const string pretty = await json.stringifyAsync(value, 1);
```

These APIs suspend their native task and perform the CPU-bound parse or
serialization on a background thread. Other tasks can continue running on the
async scheduler while a large document is processed. The captured input string
or JSON value must remain alive and unchanged until its task completes.

## Ownership and access

`parse` returns `result::<JSON>`; `parseAsync` returns `task<result<JSON>>`.
The result owns its JSON payload or Error. Matching a named result borrows the
payload; use a consuming match (`match $parsed`, `Ok(&&value)`) to transfer an
owned JSON value out. Do not return a borrowed match binding as an owned JSON.

Serialization returns an owned string. The input JSON is borrowed for the
operation, so it must outlive the task. Scheduling does not copy the document.
Typed accessors that return `&T` payloads borrow existing values; their wrappers
do not extend the lifetime of the containing document. See
[the source](../src/JSON.af) for individual accessor signatures and
[Ownership.md](../../../Ownership.md) for wrapper storage and extraction.
