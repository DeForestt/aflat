# JSON

The `JSON` module provides synchronous and native-task APIs for parsing and
serializing JSON values.

## Parsing

```aflat
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
