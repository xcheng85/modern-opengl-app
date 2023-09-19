## auto pitfall

auto deduction type is not expected: 
1. char *
2. initializer_list

auto to save iterator type 

## auto + lamda: generic lambda 

## what is trailing return types

in ts :Promise<number>

auto deduct

## type deduction with decltype

decltype(auto)
Note: decltype(auto) is primarily useful for deducing the return type of forwarding functions and similar wrappers, as shown above, where you want the type to exactly “track” some expression you’re invoking

## lvalue vs rvalue

side of assigent

## lvalue reference
int&

persistent

## rvalue reference
temporarly
int&& 

examples:
1,2,
f()


## rvalue and move 

turn something to rvalue
std::move()

## perfact forwarding in a function template
std::forward:
function params: lvalue or rvalue preserved intention

## literal operator

""s

## type alias and alias template
using fn = void(byte, double)
using fn = std::function<void(byte, double)>


template<typename T>
using aaa = std::vector<T>

## uniform initialization for direct and copy
brace-init: {}
direct init:
copy init: 

initialzer_list ctor takes highest priority

pitfall: {} could mislead you


## conversion
narrowing conversion: float to int
implicit conversion: int to float


## class/struct non-static member initialization + constructor

constructor init list

1. const non-static member
2. member does not init in the constructor 

## class init list vs assignment init 
cost: copy constructor vs 2 actions: default constructor and assignment overload

## scoped eunm 
1. avoid naming clashes
2. type could be more than int, inheritance
3. allow forward declaration possible: forward declaration is suggested in large projects

enum class xxx
using directive: less typing

## interface, virtual, override and final

## ranged-based loop, for custom class, c++ ranges library

compare with golang 
```golang
for  x,y :=range tt {

}
```
```c++
# rvalue reference
for(auto&& rate: sss()){

}
```

## class ctor: explict 
no implicit conversion

## namespace and name collision, versioning and backward compatibly with inline namespaces
unnamed namespace vs static 

## formatting text
std::format
#include <format>
need gcc 13+

## return value structural binding

tuple,map,etc.


```c++
auto [v1, v2, v3] = function();
```

```golang
fid, err := svc.getOffset(data.ID, 34)
```

```typescript
const {} = object

```

## template argument deduction. 
compiler deduction maybe not what you expect.
1. sepcify type on the lvalue = side
2. explictly create deduction rules in the same namespace to affect compiler


## conversion between string and numerical types



## optional 

#include <optional> 

?: typescript