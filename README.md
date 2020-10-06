# junhasl
A simple C++ serialization library.

Contact : junhayang1@gmail.com

## Terminologies
Before explaining anything, let me be clear about the terminologies
1. The header named ``Header_basic.h`` does some ``typedef``s for shortened names. For example, ``uchar, ushort, uint`` are for ``unsigned _``. ``long long int`` is typedef-ed as ``bint``. And also, ``unordered_map`` and ``unordered_set`` are defined as ``umap`` and ``uset``.
2. Often used ``std`` components are declared by``using``.
3. The official name of this library is 'junhaysl', but in code it's called 'ysl'.
4. 'Serialization' means both write(``to()``) and read(``from()``), but usually write. (The term 'deserialization' is more precise for reading)

## Types
You can serialize(both read/write) any type that consists only of:

1. ``char``, ``uchar``, ``short``, ``ushort``, ``int``, ``uint``, ``bint``, ``ubint``, ``float``, ``double``
2. ``string``, ``bytes`` (``vector<uchar>``)
3. ``vector<>``, ``list<>``, ``array<>``, ``umap<>``, ``uset<>``
4. ``tuple<>``
5. ``optional<>``

And also you can make any ``class`` or ``struct`` to be 'serializable' with a very simple mark.
For example,
```c++
struct SSomething : public ysl_struct_base
{
  int a; float b;
  using ysl_parent = void;
  YSL_PACK_S(SSomething, a, b);
};
```
Your custom struct ``SSomething`` will be perfectly serializable.

For example, you can serialize ``umap<int, tuple<float, vector<SSomething>, optional<string>>>`` or so.

## Usage
Extremely simple.
```c++
ysl::yobject r = ysl::to(tuple{1, vector<int>{3,4}, 3.0f, string("hi")});
// (do whatever you want with r)
tuple<int, vector<int>, float, string> q;
ysl::from(r, q);
```
The struct ``yobject`` contains a member variable ``bytes x;``. It's a byte array. You can save it in a file, send via network, compress, or whatever.

## Conversion
If you load(or read) a serialized data, you have some degree of freedom to alter the type and let it automatically adapt.
There are some rules:

### Primitive types
All primitive types (``int``, ``float``, ...) are interchangeable. 
For example,
```c++
auto r = ysl::to(tuple{int(1), 2.3f, uchar(4)});
tuple<int, float, uchar> same_one;
tuple<float, double, short> altered_one;
ysl::from(r, same_one); ysl::from(r, altered_one); // both are allowed!
```
The conversion will be conducted by C++'s default casting.

### Lists
All ``vector<>``, ``list<>`` and ``array<>`` will be regarded as a *List*.
They are all interchangeable.
For example,
```c++
auto r = ysl::to(vector<int>{1,2,3});
list<int> q;
ysl::from(r, q);
```

## Format
Serialized binary data conists of two parts: header & data.

Header area contains type information of the serialized original data.
It will be written in a text format, so you can easily read it or even manually write one. 
(This is important if you serialize the data for other languages that has no exactly corresponding types as C++)

All type (including HKTs like ``vector<>``) has its own *mark* on header.
(``YSL_SerializationConfiguration.h`` does ``#define`` about such marks) 
For example, ``vector<>`` is marked as ``[``, and ``int`` is marked as ``i``.
Then the serialized header of ``vector<int>`` will be ``[i``.
``tuple<>`` is marked as ``(N_...``. 
N means number of its entries and _ means marks of each.
For example, ``tuple<int, vector<int>>`` will be ``(2i[i``.

## Implementation
If you just want to just serialize, you never need to care about the implementation.

One of the most poweful feature of this library is the extremely simple overloading resolution.
It is based on TMP(Template Meta Programming).

(TODO)

## Requirement & Setup
No external dependencies at all. C++ 17 and standard libraries are enough.
If you wanna use this library, just merge all files to your project.
Then do ``#include "(Directory)/YSL/YSL.h"`` in source code.

## Note
As you noticed, junhasl doesn't support ``bool``. That's because of ``std``'s specialization of ``vector<bool>``.
Perhaps I can handle that but not now. You can use ``ybool`` which is just ``uchar``. If you really care the size and want to keep ``bool`` in a single bit, then I recommend you to keep such structure in ``bytes``.

There is an implementation for [glm](https://glm.g-truc.net/0.9.9/index.html) stuffs. If you don't need that, you can en/disable by modifying ``YSL_SerializationConfiguration.h``. (disabled by default)

There is also a Python3 implementation, but not included in this repo. Contact me if you want.

There are other useful features but I'll skip for now. (sorry!)




