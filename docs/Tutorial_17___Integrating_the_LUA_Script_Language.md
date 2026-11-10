# Tutorial 17 – Integrating the LUA Script Language

Original URL: http://www.drmop.com/index.php/2012/06/26/marmalade-sdk-tutorial-integrating-lua-script-language/

This tutorial is part of the Marmalade SDK tutorials collection. To see the tutorials index [click here](http://www.drmop.com/?page_id=91 "Marmalade SDK tutorials index")

Its been a while since I wrote a Marmalade SDk tutorial and to be honest I’ve been itching for the chance to start them back up, so I freed up a little time today to bring you this new tutorial.

Ok, decided that it was high time that IwGame had a proper scripting language, so took a look around various scripting languages including LUA, Javascript and AngelScript. Whilst we like them all and would like to integrate them all at some point, our first choice is LUA.

And as I am a complete LUA noob I decided to write a short tutorial showing how to integrate LUA so a) everyone can see how “not so difficult” (had to avoid the word easy, because that would be wrong) it is to integrate LUA and b) so I don’t forget how to do it myself in the future

I have created a small Marmalade app called lua\_test that demonstrates the following:

* Execute a string containing lua commands
* Load and execute a lua file from C
* Load and execute a specific function in a lua file from C
* Load a lua file that executes a C function

With these basics you should be able to integrate LUA into your own project quite easily.

I’m not going to pretend that I am now a LUA expert after spending but a few hours learning how to integrate it, but I know a few things now to be confident to write this tutorial.

Setting up the MKB to support LUA
---------------------------------

The first thing you need to do is edit your projects MKB file and the following sections:

```

packages
{
    lua
}

subprojects
{
    lua
}
```

LUA is not distributed with Marmalade, instead it is downloaded from the code repository when you run your MKB file. When you open up your MKB file you will notice a new lua filter has been added

Note that the version of LUA that is supported by the Marmalade SDK is currently 5.1.4

LUA header files
----------------

In order to access LUA you need to include the headers. However as we are including C headers into a C++ app we need to let the compiler know. With that in mind we add the headers as follows:

[sourcecode language=”cpp”]  
// LUA headers  
extern "C"  
{  
#include "lua.h"  
#include "lauxlib.h"  
#include "lualib.h"  
}  
[/sourcecode]

LUA State
---------

LUA holds no global variables so the interpreters current state has to be stored somewhere where it can be accessed from your code. In our simple examples, we create the LUA state using lua\_open() which returns a pointer to the state then close it when we have finished using lua\_close(). Ideally you would want to manage this pointer to the LUA state, depending on the scope and lifetime of LUA scripts in your app. All of our examples start and end with:

[sourcecode language=”cpp”]  
// Open LUA  
lua\_State \*lua = lua\_open();

// Add LUA libraries (standard libraries are libraries of code that provide some basic functionality such as print)  
luaL\_openlibs(lua); // See http://www.lua.org/manual/5.1/manual.html#luaL\_openlibs

//  
// Do something with LUA  
//

// Shut down the lua state  
lua\_close(lua); // See http://www.lua.org/manual/5.1/manual.html#lua\_close  
[/sourcecode]

LUA Stack
---------

LUA passes information between C and LAU code via a stack. To pass information to a LUA function from C you push the name of the function that you wish to call onto the stack followed by any parameters that you wish to pass. When the LUA function returns it returns any return values (LUA can return multiple values) or an error on the stack. Similarly when calling a C function from LUA, LUA pushes the parameters onto the stack then calls the C function. The C function examines the parameters on the stack (C does not remove them, LUA removes them when it returns) and utilises the data passed from LUA, carries out some logic and returns.

More info on the LUA stack can be found at http://www.lua.org/pil/24.2.html

Calling a string containing LUA from C
--------------------------------------

Here’s some example code showing how to execute a piece of LUA contained within a string:

[sourcecode language=”cpp”]  
void CallLuaString(const char\* string)  
{  
s3eDebugOutputString("==== Calling a LUA string");

// Open LUA  
lua\_State \*lua = lua\_open();

// Add LUA libraries  
luaL\_openlibs(lua); // See http://www.lua.org/manual/5.1/manual.html#luaL\_openlibs

// Pass the string to lua to execute  
if (luaL\_loadbuffer(lua, string, strlen(string), "line") == 0) // See http://www.lua.org/manual/5.1/manual.html#luaL\_loadbuffer  
{  
if (lua\_pcall(lua, 0, 0, 0) != 0)  
{  
// Output the error  
s3eDebugOutputString(lua\_tostring(lua, -1));

// Pop error message off the stack  
lua\_pop(lua, 1); // see http://www.lua.org/manual/5.1/manual.html#lua\_pop  
}  
}

// Shut down the lua state  
lua\_close(lua); // See http://www.lua.org/manual/5.1/manual.html#lua\_close  
}  
[/sourcecode]

In this example, we open LUA then ad the standard LUA libraries. We then load the supplied string as a LUA chunk using luaL\_loadbuffer() then call it using lua\_pcall(). If an error occurs then we display the error and pop it off the stack

Calling a LUA file from C
-------------------------

Our next example looks at how to call a LUA file from C, lets take a look at the code:

[sourcecode language=”cpp”]  
void CallLuaFile(const char\* lua\_filename)  
{  
s3eDebugOutputString("==== Calling a LUA file");

// Open LUA  
lua\_State \*lua = lua\_open();

// Add LUA libraries  
luaL\_openlibs(lua); // See http://www.lua.org/manual/5.1/manual.html#luaL\_openlibs

// Load our test LUA file  
if (luaL\_loadfile(lua, lua\_filename) == 0) // See http://www.lua.org/manual/5.1/manual.html#lua\_load  
{  
if (lua\_pcall(lua, 0, 0, 0) != 0) // See http://www.lua.org/manual/5.1/manual.html#lua\_pcall  
{  
// Output the error  
s3eDebugOutputString(lua\_tostring(lua, -1));

// Pop error message off the stack  
lua\_pop(lua, 1); // see http://www.lua.org/manual/5.1/manual.html#lua\_pop  
}  
}

// Shut down the lua state  
lua\_close(lua); // See http://www.lua.org/manual/5.1/manual.html#lua\_close  
}  
[/sourcecode]

We begin this example much like we did our previous example, but instead of load\_buffer() we call luaL\_loadfile(), which basically does the same thing except it loads the data from a file.

Calling a LUA function from C
-----------------------------

This example gets a little more complicated as we need to begin dealing with pushing data onto the LUA stack. Lets take a look at the code:

[sourcecode language=”cpp”]  
void CallLuaFunctionInFile(const char\* lua\_filename, const char\* function\_name, double arg0, double arg1)  
{  
s3eDebugOutputString("==== Calling a LUA function from C");

// Open LUA  
lua\_State \*lua = lua\_open();

// Add LUA libraries  
luaL\_openlibs(lua); // See http://www.lua.org/manual/5.1/manual.html#luaL\_openlibs

// Load our test LUA file  
if (luaL\_loadfile(lua, lua\_filename) == 0) // See http://www.lua.org/manual/5.1/manual.html#lua\_load  
{  
// Init the loaded lua file  
if (lua\_pcall(lua, 0, 0, 0) == 0) // See http://www.lua.org/manual/5.1/manual.html#lua\_pcall  
{  
// Push the name of the function that we want to call onto the stack  
// NB: LUA uses a stack to pass information back and forth between LUA and C (see http://www.lua.org/pil/24.2.html)  
lua\_getglobal(lua, function\_name); // Push function name that we want to call ontothe stack (see http://www.lua.org/manual/5.1/manual.html#lua\_getglobal)  
lua\_pushnumber(lua, arg0); // Push function call argument 1  
lua\_pushnumber(lua, arg1); // Push function call argument 2

if (lua\_pcall(lua, 2, 1, 0) != 0) // See http://www.lua.org/manual/5.1/manual.html#lua\_pcall  
s3eDebugOutputString(lua\_tostring(lua, -1));  
else  
{  
if (!lua\_isnumber(lua, -1)) // http://www.lua.org/manual/5.1/manual.html#lua\_isnumber  
s3eDebugOutputString("Function muust return a value");  
else  
{  
// Get th result returned from the LUA function  
double result = lua\_tonumber(lua, -1);  
}  
}  
lua\_pop(lua, 1); // see http://www.lua.org/manual/5.1/manual.html#lua\_pop  
}  
}

// Shut down the lua state  
lua\_close(lua); // See http://www.lua.org/manual/5.1/manual.html#lua\_close  
}  
[/sourcecode]

Much of the code remains the same as the previous example, where we initialise LUA, load a LUA file and execute it. The difference is that we now push an actual function name and a number of parameters onto the stack using lua\_getglobal() and lua\_pushnumber(). We then make another call to LUA which executes the LUA function that we just stacked, e then check the return value and if itis not a number we display an error otherwise we pop the returned number off the stack and store it locally.

Calling a C function from LUA
-----------------------------

Things get a little more complicated when you find that you need to call C functions from LUA. The first thing that we need to do is let LUA know that our C function is available to be called as well as define a protocol for how it should be called from LUA. Lets take a look at an example:

[sourcecode language=”cpp”]  
static int test\_function(lua\_State \*lua)  
{  
double d = 0;  
// Get the argument that was passed from lua  
if (lua\_isnumber(lua, 1))  
d = lua\_tonumber(lua, 1);  
else  
s3eDebugOutputString("test\_function can only accept a number");

// Square it  
d = d \* d;

// Push the result back onto the stack as a return value  
lua\_pushnumber(lua, d);

// Return the number of result arguments that were passed back to lua  
return 1;  
}

void CallCFunctionFromLua()  
{  
s3eDebugOutputString("==== Calling a C function from LUA");

// Open LUA  
lua\_State \*lua = lua\_open();

// Add LUA libraries  
luaL\_openlibs(lua); // See http://www.lua.org/manual/5.1/manual.html#luaL\_openlibs

// Push the test function  
lua\_pushcfunction(lua, test\_function); // See http://www.lua.org/manual/5.1/manual.html#lua\_pushcfunction

// Set the global test\_function to the pushed C function  
lua\_setglobal(lua, "test\_function"); // See http://www.lua.org/manual/5.1/manual.html#lua\_setglobal

// Load our test LUA file  
if (luaL\_loadfile(lua, "lua\_test3.lua") == 0) // See http://www.lua.org/manual/5.1/manual.html#lua\_load  
{  
if (lua\_pcall(lua, 0, 0, 0) != 0) // See http://www.lua.org/manual/5.1/manual.html#lua\_pcall  
{  
// Output the error  
s3eDebugOutputString(lua\_tostring(lua, -1));

// Pop error message off the stack  
lua\_pop(lua, 1); // see http://www.lua.org/manual/5.1/manual.html#lua\_pop  
}  
}

// Shut down the lua state  
lua\_close(lua); // See http://www.lua.org/manual/5.1/manual.html#lua\_close}  
}  
[/sourcecode]

Firstly we create a C function called test\_function() which is declared from LUA’s standard C function prototype:

static int test\_function(lua\_State \*lua)

This function accepts a lua state which we can query for information that was passed to it by LUA (on the stack). In our example, we firstly check to see if the value on the top of the stack is a number, if not we display an error, if so then we retrieve that number. Next we modify the number then push it back onto the stack so that it can be retrieved (returned to) by the calling LUA function. Note that the value returned from our C function represents the number of arguments that we returned on the stack. Lets take a quick look at the LUA code that made the call to our C test\_function in the first place:

```

d = test_function(12);
print(d);
d = test_function("hello");    -- This will cause an error because we passed a string instead of a number
print(d);
```

As you can see it calls test\_function twice. The first time it is successful because we passed a number, the second time we passed a string which test\_function() detected as an error.

Ok, lets now take a look at the code that registers our C function so that LUA can use it (see CallCFunctionFromLua() above)

Firstly we call lua\_pushcfunction(lua, test\_function); which places our C function onto the stack. Next we call lua\_setglobal(lua, “test\_function”); to assign the function an actual name that LUA script can use to call the function.

Well that’s about it. I have included the source code for the lua\_test app which you can [download from here](http://www.drmop.com/wp-content/uploads/2012/06/LUA.zip "Marmalade SDK Tutorial -  Integrating LUA Script Language Source Code")