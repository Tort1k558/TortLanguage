# Functions

Functions in the language are declared and defined in this way
```
def nameOfFunction(args...)
{
	return something;
}
```
The type of the returned value does not have to be specified, it is automatically calculated at the compilation stage of the program if possible, if the compiler cannot calculate the type of the returned value, then the type must be specified explicitly, like this
```
def type nameOfFunction(args...)
{
	return something;
}
```
Also, the function may not return values and it is not necessary to write the return keyword.
```
def nothing()
{

}
```

### Example
```
def sum(int a,int b)
{
	return a + b;
}

def double pi()
{
	return 3.1415926;
}
```