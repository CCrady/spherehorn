# A Guide to Spherehorn
*Note: this guide assumes that you already know a bit about programming language
design in general, and esoteric programming languages in particular.*

## The Structure of a Program
Spherehorn's design is based on a particular data structure, a 'circular tree'.
It's a tree where every branch node can have an unlimited number of children,
and those children are arranged in a circular, looping list. Code and memory in
Spherehorn both follow this kind of structure.

A Spherehorn program has three parts: the code, the memory, and the program
state.

### Program state
Spherehorn programs have access to an **accumulator register**, a **conditional
register**, and a **memory pointer**. The accumulator stores an unsigned
integer, and the conditional stores a boolean. The memory pointer points to a
particular node in the program memory, which is said to be the 'current' node.

### Program code
Code in Spherehorn is made of instructions arranged inside looping blocks. Take
the following example, from `counter.spherehorn`:
```
{
    numin
    >
    {
        .a
        numout
        >
        chout
        >
        >= m; break?
        >
        ++
    }
    break
}
```
The `{` and `}` symbols here indicate the starts/ends of code blocks, as in many
other languages. Ignore the other symbols for now; those are instructions.

Code blocks can be nested as you'd expect. When control flow reaches a `{`, it
will enter the block and start executing the instructions inside, from the first
one written onwards. In Spherehorn, all code blocks are loops, so when control
flow reaches a `}`, it will return to the matching `{` and the program will
continue executing from there.

### The memory tree
Memory in Spherehorn takes the form of a circular tree where each node has an
associated value. Here's an example:
```
(
    1
    2
    (
        0
        1
    )
)
```
`(` and `)` indicate the starts/ends of nodes of the memory tree, and values are
separated by whitespace; this syntax should be familiar if you've ever used
Lisp. The list of a node's children is circular, so the last child's next
neighbor is the first child (and the first child's previous neighbor is the last
child).

A memory node's value is the number of children it has, so the node `( 0 1 )`
has a value of `2`, and the root node in the example has a value of `3`.
However, this also applies in reverse: a memory node's value determines the
number of children it has. So a node written as `0` means a node with no
children, `()`. Nodes default to this empty state. A node written `2` has two
children, but those childrean don't have an explicit value specified, so the
node is equivalent to `( () () )`. Here's what the example above looks like when
it's expanded into this paren-only form:
```
(
    ( () )
    ( () () )
    (
        ()
        ( () )
    )
)
```

## Other Important Concepts

### Values and literals
Spherehorn deals with two different kinds of values: **numeric values** and
**memory values**. Numeric values are typical unsigned integers. Memory values,
on the other hand, represent the contents of a memory node. For the most part,
you'll see numeric values in code blocks and memory values in memory blocks (the
exception being the [memory setter instruction](#the-memory-setter)).

Spherehorn has several different ways of writing literal values, but they all
end up as either numeric or memory values.

#### Numeric literals
Numeric literals represent a numeric value. If they're used in a place that
expects a memory value, such as in a memory block, they're automatically
converted into a memory node whose value is the numeric value and whose children
are all empty nodes. The following types of literals are numeric:
- Integer literals, written like `1234`. These may optionally take a prefix to
  indicate the radix: `0b` for binary, `0o` for octal, `0d` for decimal, and
  `0x` for hexadecimal. If no prefix is included, the literal will default to
  decimal.
- Character literals, written like `'x'`. The number that the character
  corresponds to is its 8-bit unsigned ASCII representation. Backslash escapes
  are allowed; e.g. `'\n'` represents a newline character, aka the number 10.
- Boolean literals. A boolean literal can either be `T` (true, the number 1) or
  `F` (false, the number 0).

#### Memory literals
There are two types of literals which can only represent memory values, not
numeric ones. They specify the contents of the memory node, rather than just its
size.
- Memory blocks. We've already seen some examples of what a memory block literal
  looks like (see [*The memory tree*](#the-memory-tree)). They begin with a `(`
  and end with a `)`, and contain zero or more literals representing the memory
  node's children. The child literals may be delimited by whitespace, and this
  is mandatory in cases where not including delimiters would cause ambiguities.
  The first literal written in the memory block is considered to be the first
  child. The second one is likewise considered the second child, and so on.
  Memory blocks may not contain anything besides literals.
- String literals, written like `"FOO"`. A string literal represents a memory
  node whose children are the characters of the string, so the literals `"FOO"`,
  `('F' 'O' 'O')`, and `(70 79 79)` all mean the same thing.

### Instructions
An **instruction** is a bit of Spherehorn code that tells the program to do
something specific. Syntactically, instructions take the form
`CODE [ARGUMENT] [TERMINATOR]`. The argument, if one exists, must be separated
from the code by whitespace. The terminator may or may not have whitespace
before it. For a complete list of instruction codes and what they do, see
[*List of Instructions*](#list-of-instructions) below.

**Arguments** to instructions may be [numeric literals](#literal-values), or
they may be either of the special symbols `a` or `m`. The numeric values of
these symbols depend on the values of the accumulator and the current memory
node, respectively. In other words, `a` is a variable meaning "the value of the
accumulator", and `m` is a variable meaning "the value of the currently
pointed-to memory node". Arguments may not be memory block literals or string
literals, since these evaluate to memory nodes rather than numbers. The one
exception is the memory setter instruction `.` (see
[*The memory setter*](#the-memory-setter)).

### Terminators and conditionals
The characters `;`, `?`, and `!` are **terminators**. They can be applied to
instructions or code blocks, and are used for conditionals. When applied to an
instruction they come after the code and argument, and when applied to a code
block they come after the opening `{`.

If you use a `?` terminator, the associated instruction or code block will only
be executed if the conditional register is set to true. The `!` terminator is
similar, but it causes the statement to only be executed if the conditional
register is false. And if you don't include a terminator, or you use the `;`
terminator, the corresponding instruction or code block will be executed
unconditionally. The `;` terminator is never required, since there are no
ambiguities when parsing instructions, but it can be used to break up
instructions visually.

When an instruction or code block isn't executed because of a conditional,
control flow will skip past it, moving to the next instruction/block.
Conditionals on blocks are only evaluated once, when the block is entered. Once
control flow has entered a block, the only way to exit it is with a `break`
instruction.

### Program syntax
Every Spherehorn program must have a [code](#program-code) block containing one
or more statements, and a [memory](#the-memory-tree) block containing one or
more memory nodes. These must appear at the top-level scope, i.e. not within any
other code or memory blocks, but either one can come first. When the program
starts executing, control flow will start at the first statement of the
top-level code block, and the memory pointer will point to the first child of
the top-level memory block.

A Spherehorn program may optionally specify the initial values of the
accumulator and conditional registers via the syntax `a: X` and `c: X` (where
`X` is some numeric literal). These must also appear at top-level scope, and can
appear in any order with each other and the code and memory blocks. If the
program does not specify the inital values, they will default to zero/false.

## List of Instructions
In the following examples, `X` stands for the argument of an instruction (either
a numeric literal or one of the variables `a`/`m`). If there is no `X`, the
instruction doesn't take an argument.

### Conditional and accumulator setters
- `A X` - set the accumulator to the value of `X`
- `C X` - set the conditional register to false if `X` is zero, and to true
  otherwise

### Math Operations
- `++` - increment `a`
- `--` - decrement `a`
- `+ X` - add `X` to `a` and store the result in `a`
- `- X` - subtract `X` from `a` and store the result in `a`
- `r- X` - subtract `a` from `X` and store the result in `a`
- `* X` - multiply `a` with `X` and store the result in `a`
- `/ X` - divide `a` by `X`, rounding down, and store the result in `a`
- `r/ X` - divide `X` by `a`, rounding down, and store the result in `a`
- `% X` - take `a` mod `X` and store the result in `a`
- `r% X` - take `X` mod `a` and store the result in `a`

Any operation that would result in a negative number (e.g. `--` when `a` is 0)
or a division by zero (e.g. `r% X` when `a` is 0) will result in an error.

### Comparisons
- `= X` - set `c` to whether `a` is equal to `X`
- `>> X` - set `c` to whether `a` is greater than `X`
- `<< X` - set `c` to whether `a` is less than `X`
- `>= X` - set `c` to whether `a` is greater than or equal to `X`
- `<= X` - set `c` to whether `a` is less than or equal to `X`
- `/= X` - set `c` to whether `a` is not equal to `X`

### Boolean operations
- `and X` - set `c` to the value of the boolean expression "`c` and `X`"
- `or X` - set `c` to the value of the boolean expression "`c` or `X`"
- `xor X` - set `c` to the value of the boolean expression "`c` xor `X`"
- `not` - invert the value of `c`

### Moving the memory pointer
- `<` - move the memory pointer one place backward in the current loop
- `>` - move the memory pointer one place forward in the current loop
- `< X` - move the memory pointer `X` places backward in the current memory loop
- `> X` - move the memory pointer `X` places forward in the current memory loop
- `^` - move the memory pointer to the parent of the current memory node
- `v` - move the memory pointer to the first child of the current memory node
- `R` - reset the memory pointer to the first node of the current loop

Attempting to use `v` on a childless memory node will result in an error. Using
`^` on the top level of memory will cause the program to exit.

### Manipulating memory
- `rot` - make the current memory node the first one in its loop, thereby
  'rotating' the loop
- `<+` - insert a new memory node just before this one, and move the memory
  pointer to it
- `+>` - insert a new memory node just after this one, and move the memory
  pointer to it
- `<-` - delete the current memory node, and move the memory pointer to its
  previous neighbor
- `->` - delete the current memory node, and move the memory pointer to its next
  neighbor

Using `<-` or `->` when there's only one memory node in the current loop will
move the memory pointer to that node's parent (since it no longer has any
neighbors).

### Break
`break` is a special instruction that causes a program's control flow to break
out of the current code block, the same way the `break` keyword works in most
other languages. It tends to be most useful when it's conditional, as a way to
implement a while loop.

### The memory setter
The memory setter differs from other instructions in a few ways. It's written as
a dot `.` before a value (literal or variable), e.g. `.a` (whitespace between
the `.` and the value is optional). As well as numeric values, you can use
memory node literals, e.g. `."foo"`. When a memory setter instruction executes,
it will clear the current memory node and set it to whatever its value is.

As an example, let's say that at some point in the execution of a program the
memory tree looks like this:
```
(
    3
  > (
        T
        'A'
    )
    "foo"
)
```
(The `>` symbol marks the current memory node.) Then we execute a memory setter
instruction: `. ( 0 1 2 ( 3 ) )`. Now the memory looks like this:
```
(
    3
  > (
        0
        1
        2
        (
            3
        )
    )
    "foo"
)
```

