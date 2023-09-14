std::optional

has_value

like ?: in ts


std::clamp



## use case: search through data

1. std::find()

2. std::find_if(), takes a lamda

3. std::find_if_not()

// range in another range
4. find_first_of

5. find_end()

6. std::binary_search, return boolean

7. lower_bound() // not less than >= 

8. upper_bound() // < 

9. equal_range: combo of 7,8


## use case: sort range

1. sort()

2. stable_sort()

3. partial_sort()

4. nth_element, 

5. is_sorted() 

6. is_sorted_untial


## use case: distance between two iterators

1. std::distance()

## use case: initialize values of containers. similar to lodash

1. std::fill: 

2. std::generate(): 

3. std::iota

## use case: operation on two containers and mutate output to third one

1. merge
2. union
3. intersection
4. subrange


## use case: insert new element to container

std::back_inserter
std::front_inserter
std::inserter()

## stl member function vs non-member function


## std::any and std::optional, std::variant

std::any , ts any
std::optional, ts ?
std::variant ts |

## std::span


## use case: function return result
1. return int indicating status code, enum class
2. return bool
3. return std::optional

## use case: exceptions usage in constructor and destructor

constructure no return value, how to indicate failure
exceptions

RAII idiom

excpetion does not leave out of dtor


## implement custom exception class


## type-casting

## smart pointer

unique_ptr
make_unique
unique_ptr and std::move to transfer ownership
unique_ptr and vector container

virtual dctor in base class key


## moving 
move ctor
assignment operator

