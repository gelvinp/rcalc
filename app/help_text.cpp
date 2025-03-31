#include "help_text.h"

namespace RCalc {

const char* HelpText::program_info =
R"foo(RCalc is an RPN (Reverse Polish Notation) calculator that aims to be quick to open and use, and easy to extend with new types, operators, and commands.

This software is developed by Patrick Gelvin, and released under the MIT license. For more information about licensing, and the licenses of the components used to make RCalc, scroll to the bottom of this window.
)foo";


constexpr HelpText::HelpSection HelpSections[] =  {
    {
        "Reverse Polish Notation",
R"foo(Most calculators use a style of notation similar to '1 + 2 = 3', known as _infix_ notation. Reverse Polish Notation, or RPN for short, is instead a _postfix_ notation, where operators are placed after their operands (i.e. '1 2 + = 3'). Intuitively, operands are placed into a _stack_, and then operators operate off the top of the stack.

To understand how this works in RCalc, let's examine the `add` operator. Add takes two arguments, referred to as arg0 and arg1, and returns their sum. If you type 1 <enter> 2 <enter>, the stack will have two values. In this example, 1 will become arg0 and 2 will become arg1. Applying the add operator to this stack gives us `1 + 2`.

For a more complicated example, 6 - (4 * sin(pi/2)) would be entered as:
6 <enter> 4 <enter> pi <enter> 2 div <enter> sin <enter> mul <enter> sub <enter>

Notice how we enter our operands first, from the outermost inward, and then apply our operators second, from the innermost outward. People who become proficient at using RPN can enter calculations with great speed.
)foo"
    },
    {
        "Entering Values",
R"foo(RCalc supports a variety of value types and formats, from integers and reals to vectors and matrices.

To enter numbers, you can type them like you normally would. Below are some examples of valid numbers:
123
76.8
0.4
0x80 (Parsed as 128)
0o24 (Parsed as 20)
4e6 (Parsed as 4000000)

Please note that pressing the '-' key may be treated as immedately calling for the subtract operator (See more in the Entering Operators section below), therefore to enter negative numbers, please use the `n` prefix instead:
n45 (Parsed as -45)
6en24 (Parsed as 6e-24)

You can additionally press the up and down arrows in the scratchpad to scroll through the history of values, operators, commands, and units that you have entered.

If the scratchpad is empty, and you press Enter, the top element on the stack will be duplicated. Similarly, if the scratchpad is empty and you press Delete, the top element on the stack will be removed.
)foo"
    },
    {
        "Entering Operators",
R"foo(RCalc supports many different operators which are invoked using their name. For example, to calculate the cosine of pi radians, type
pi <enter> cos <enter>

You can also press tab in the scratchpad to autocomplete operators that are available based on the types of values currently in the stack.

The ImGui and Terminal renderers will also treat the keys '+', '-', '*', and '/' as entering their respective operators. When this happens, anything in the scratchpad will automatically be pushed to the stack. Therefore, to quickly perform simple arithmetic, such as (1 + 2) * 3, you can quickly type
1 <enter> 2 + 3 *

Further down on this help screen is: a list of all operators, a brief description for each operator, the number of types, and which types, each operator requires, and example(s) of inputs and outputs for each operator.
)foo"
    },
    {
        "Entering Commands",
R"foo(RCalc supports commands at different scopes. Some commands will be available regardless of the renderer, while other commands are renderer specific. You can see a list of all commands and a brief description for each command, separated by scope, further down on this help screen.

You can also press tab in the scratchpad to autocomplete commands that are available.

To use a command, enter a backslash ('\') followed by the name of the command. Some commands have alias(es) listed, for example you can enter `\quit` to quit RCalc, but you can also enter `\q` to accomplish the same task.
)foo"
    },
    {
        "Entering Vectors",
R"foo(RCalc supports vectors of 2, 3, and 4 components. To enter vectors, wrap its components in square brackets:
[1, 2]
[6, n4, 9]
[0xfe, 8, n1, 2e9]

You can also create vectors from values on the stack using the vec2, vec3, and vec4 operators, or join scalars and vectors together using the concat operator.

Vectors can be _swizzled_, allowing you to access and rearrange components in a vector. When a vector is at the top of the stack (bottom of the screen), enter a period ('.') followed by 1-4 of either xyzw or rgba to select the first, second, third, or fourth component of the vector. Below are some examples of swizzled vectors:
[1, 2].yx -> [2, 1]
[1, 2, 3].xzy -> [1, 3, 2]
[1, 2, 3, 4].rrab -> [1, 1, 4, 3]
[1, 2].rrgg -> [1, 1, 2, 2]
[1, 2, 3, 4].rgb -> [1, 2, 3]
)foo"
    },
    {
        "Entering Matrices",
R"foo(RCalc supports square matrices either 2x2, 3x3, or 4x4 in size. To enter matrices, wrap 2-4 row vectors in curly braces:
{[1, 2], [3, 4]}
{[1, 0, 0], [0, 1, 0], [0, 0, 1]}
{[4, 8, n3, 5], [n2, n2, 4, 0], [0x12, 9, 4, 3en4], [0, 1, 32, 9]}

You can also create matrices from row vectors on the stack using the mat2, mat3, and mat4 operators.
)foo"
    },
    {
        "Unit Conversions",
R"foo(RCalc supports converting between units in the same family. To enter a unit, enter an underscore ('_') followed by its abbreviation. For example:
_ft (Feet)
_lbs (Pounds)
_c (Celsius)
_polar (Polar r and theta)
_oklch (OKLCh color space)

You can also press tab in the scratchpad to autocomplete units that are available based on the types of values currently in the stack.

You can see a list of all unit families, the base type used by each family, and the unit name and usage for each unit that belongs to the family further down on this help screen.

To convert a value, push it onto the stack, followed by the unit you want to convert from, then the unit you want to convert to, then use the convert operator. For example:
78 <enter> _f <enter> _c <enter> convert <enter> = 25.5556
[1, 1.5708] <enter> _polar <enter> _cartxy <enter> convert <enter> = [0, 1]
[200, 164, 237] <enter> _rgb <enter> _hsl <enter> convert <enter> = [269.589, 66.9725, 78.6275]
)foo"
    },
    {
        "Using Variables",
R"foo(You can store the results of your calculations in variables for later use by storing it with an Identifier. An Identifier is any value that begins and ends with double quotes ("").
Identifiers are NOT case sensitive, and CANNOT be empty.

Use the \store command to store a value with an Identifier, for example:
6.28 <enter> "tau" <enter> \store <enter>

Now, you can use the the \load command to retrieve the value, for example:
"tau" <enter> \load <enter>
will push the value 6.28 back onto the stack.

You can also use variables directly in calculations, for example:
"tau" <enter> 2 /
will give the result 3.14.
)foo"
    }
};

const std::span<const HelpText::HelpSection> HelpText::sections = { HelpSections };

}