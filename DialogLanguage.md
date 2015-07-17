# Introduction #

To ease rapid prototyping of dialog, we use a purpose-built interpreted language much simpler than C.

The goals are simplicity, flexibility, compatibility with existing codebase.

There is little syntax so the author can focus on the text.

Several features we do not expect to use are left unimplemented. These are noted in this document.

## basic structure ##

There are five basic large-scale entities, each declared on a line beginning with its (lowercase, singular) name.

  * Greetings show what they say to you at the beginning of a conversation, then direct you to a node of possible responses.

  * Nodes organize what you can say. They contain options. Once you declare a node, all options are in that node until you declare another one.

  * Options shows what the player-character may say on one line then what the other character would say in response on the rest of the lines, then direct you to a node.

  * Text is used for repeated or conditional phrases. You can put it anywhere, but it's best to put it either at the end or near where it'll be used.

  * Inits are used to do things before selecting greetings. You can put it anywhere, but at the beginning makes by far the most sense.

Every node and text has a name. Options may have names. Names should only include letters, numbers, underscores, and parentheses (NOTE: we can clear additional characters if requested, but that's all we guarantee right now)

Example:

<pre>
node main<br>
</pre>

or

<pre>
text get_you_for_this<br>
</pre>

This is so they can be jumped to (for nodes) or called (for text). Options may also have names. Greetings do not get names. At the end of a greeting or option, the last line is a declaration of what node to go to, in the format

<pre>
.foo<br>
</pre>

with foo the name of the node to go to. Text items just end with a line that is a '.' (if you want to have a dialog line that begins with a '.', such as '...',  we'll get to that with display directives)

There are two predefined nodenames 'done' and 'fight' with the obvious meanings.

Very simple example, where they say hello? and you can say hello! or goodbye!, then they say goodbye, then you go back to navigation.

<pre>
greeting<br>
hello? // I didn't mention this, but '//' can be used for comments<br>
.main     // leading and trailing whitespace are ignored.<br>
// You can use an indenting convention if you like, as shown below.<br>
// I don't. I just use blank lines to keep things organized.<br>
<br>
node main<br>
option<br>
hello!<br>
goodbye<br>
.done<br>
<br>
option<br>
goodbye!<br>
goodbye<br>
.done<br>
</pre>

You CAN but probably shouldn't have blank lines in the middle of your statements.
If I write a validation utility, that will definitely give a warning.

## Conditions and Consequences ##

Any of these items -- greeting, node, option, or text -- may have _conditions_ and _consequences_, which are listed on the same line, delimited by spaces. The conditions and consequences are indicated by single-character symbol followed by an expression. The simplest expressions are simply a single name - the key to an integer value. The default value for all keys is 0, and corresponds to 'false'. Any other value is considered 'true'. If a key will only have a value of 1 or 0 - 'true' or 'false' - we call it a flag. The language is heavily optimized for dealing with this simplest case.

If all of the conditions on an item are met, it can be used. If an item is actually used (if the search reaches it and its conditions are met - and, for options, the player also selects it), then the consequences are applied. (NOTE: We have not yet implemented consequences for entering a node, but can if desired)

This example represents the basic conditions and consequences. These are flags which can be 'set' (value = 1) or 'unset' (value = 0). There is no syntactic distinction between flags and other key-value pairs, but this keeps it easier to think about them.

<pre>
option =requiredToBeTrue !requiredToBeFalse +setThis -unsetThis<br>
</pre>
To be explicit, if the 'requiredToBeTrue' flag is unset (key has a value of 0), this option will not appear. If the 'requiredToBeFalse' is set (key has a nonzero value), this option will not appear. Otherwise, it will appear. If this option is chosen by the player, the flag 'setThis' is set (set to 1 if 0), and the flag 'unsetThis' is unset (set to 0).

Some items have names. For instance, after alien dialog, there's a '.nodename' line that tells the game to look for a node named 'nodename'. If an item is being sought _by name_ and does not meet the conditions, the search goes on for another item of the same name. This happens with nodes and texts.

The effect of giving an _option_ a name is a bit different - after an option with a given name has been accepted, any more options with the same name are automatically rejected. This is useful for presenting variations on the same option - different responses to the same player-character statement, say.

Here is a longer example illustrating the use of named conditions.

<pre>
node main<br>
option juggling =EXPERT_JUGGLER<br>
Can I show off my juggling talents?<br>
Certainly!<br>
.juggling // after this, go to node 'juggling'<br>
<br>
option juggling<br>
I don't know anything about juggling. Can you help me?<br>
Not really, sorry.<br>
.main<br>
</pre>

The second option has no condition. This makes it complementary to the first - it will appear if and only if the first one does not. The player _will_ be able to say something about juggling, and only one thing. Arranging it this way helps avoid errors - less typing out of explicit conditions, and a validator can check for option names that are used only once. Speaking of which, there is no reason to use any option name only one time - if you would, just put it in a comment!

Using a catch-all is important! If no greeting, node, or option can be found for which the conditions are met, so that literally nothing happens next, this is an _improper condition_. It will result in unprepared conversation termination and an error message.

As shortcuts and to minimize the opportunity for errors, there are three compound operations. Each pair of lines is equivalent:

<pre>
option >foo // think 'arrow of time leads to this'. Only applies if not already true, then it becomes so.<br>
option !foo +foo<br>
<br>
option <foo // think 'eat this'. Only applies if already true, then it isn't anymore.<br>
option =foo -foo<br>
</pre>

This is commonly used to close off an option so you can't take it over and over again in the same conversation. For example,

<pre>
option >ta_juggling // 'ta_' is an abbreviation I use for 'talked about'<br>
I don't know anything about juggling. Can you help me?<br>
Not really, sorry.<br>
.main<br>
</pre>

The last special symbol is the toggle, ~. It is just a consequence, not also a condition. If this consequence is applied, it first checks the key. If the key evaluates as true, it unsets the key. If the key evaluates as false, it sets it. For flags, this is what you expect from a toggle (if the keys are algebraic, it still has meaning).

<pre>
text ~Switch_on<br>
Okay, I hit the switch. Now what?<br>
.<br>
</pre>

### keys ###

The names of keys may contain the same characters as names of items - letters, numbers, underscores, parentheses (NOTE: and nearly anything else authors ask for, but DO ask).
There are three basic kinds of key, and one special one:

  * All-lower-case keys will automatically be unset at the end of the conversation - they have 'temporary' scope.
  * All-upper-case keys are global, shared by all dialogs and the game as a whole. (NOTE: For the time being, these will be cast to a BYTE, so don't try to go over 255 or less than 0)
  * Mixed-case keys are preserved but namespaced to this dialog file so you don't collide with the flags other people are using for their races. (NOTE: these are also cast to BYTE)
  * There is a key named '%', which has special behavior. See below.

Examples:
<pre>
>said_hi // use this to avoid letting the player say hi more than once per conversation<br>
// (it starts unset, thus satisfying the condition, but once taken, sets it, so it won't appear anymore).<br>
// This is a super-common idiom.<br>
<br>
>Mentioned_KohrAh // These folks will remember that the player mentioned the Kohr-Ah,<br>
// but other folks in other conversation documents won't<br>
// note the removal of the - sign, which would be seen as arithmetic.<br>
<br>
=SHIP_IS_MK_II // When the player got the MK 2 vessel, this was set<br>
</pre>

### Numbers ###

Keys do not need to have values of just 1 or 0. They can have integer values, too. You can perform arithmetic on them and test the results. For instance,
<pre>
=BioData>99<br>
</pre>

If arithmetic is being used as a **condition**, it evaluates the statement and sees whether it's 0 or not (0 is false). For example, the condition above will be met if the `BioData` numeric flag has been set to at least 100, so that the '>' operator returns '1' instead of '0'.

Arithmetic can be used in **consequences** as well. If the consequence is a 'set' consequence (+ or > or sometimes ~) involving arithmetic, then the left hand side of the statement must be a key, followed directly by a comparator (= or > or <). The right hand side will be evaluated, and the value for the key on the left hand side will be set to the nearest consistent value. The simplest and most common case is '=', which simply sets it to the value of the other side.

If the 'unset' consequence (- or < or sometimes ~) is applied, then the left-hand side is simply set to 0 without evaluating the right hand side. If the '-' consequence is used, you may (and probably should) leave off the comparator and right hand side.

Some keys are manipulated by non-dialog elements of the game, and some of those cannot be set directly, like, say, FREE\_CARGO\_CAPACITY, if there is such a thing. A list of these will eventually be placed here.

### Arithmetic ###

Numbers need not be literal - you can evaluate expressions.

The available arithmetic operators are, in order by increasing precedence:
  1. =, <, > (check for this condition being satisfied, return 1 or 0 appropriately)
  1. +, -
  1. /
  1. `*`
  1. ^, √ (√ is always a binary operator. '2' is not assumed). These two operators resolve RIGHT to LEFT

> `a*b/c*d` is just what it looks like - d is in the denominator, not the numerator, because `*` binds more tightly than /. This choice was made so the trimming-to-integer is minimally invasive - numbers are made as large as possible before they get divided. (NOTE: I am relying on the idea that numbers will be small compared to the maximum size of an int. Be warned that this could end up messy if this assumption is violated)

The radical and exponent operators are arranged as you'd expect, looking at them - and they put the base on opposite sides. So,  ` a√b^c ` is what we'd normally call ` b^[c/a] `, except that c/a would be trimmed to an integer if you did it that way. Note that doing things right-to-left helps here so the exponentiation acts before the radical, without having to use grouping.

Speaking of which, use square brackets for grouping, like ` =2√[[MAP_X-500]^2+[MAP_Y-500]^2]<10 `. This condition is satisfied if you're within 10 units of the center of the map, not inclusive (if you want to make it inclusive, use '!' and '>' instead of '=' and '<' ).

Parentheses are ignored so you can treat them as regular text without having to worry about it being script-language syntax.

You may be tempted to put spaces in your expressions. Don't. It'll mess up the parser. Sorry.


### randomness ###

There's a special key with a randomly determined value from 0 to 99. It is not rerolled when you evaluate it, but only at the beginning of the conversation and if you try to set it or unset it. This key is, simply, %.

<pre>
text blah <%<25 // this meets the condition 1/4 of the time<br>
BLAH!!!<br>
.<br>
<br>
text blah <%<50 // this also meets the condition 1/4 of the time<br>
//because it won't be taken if the first one was because it's named blah like the first one<br>
blah<br>
.<br>
<br>
text blah -% // this meets the implicit condition half the time<br>
buh<br>
.<br>
</pre>
Note that instead of using the = condition, the < condition was used to require it... and then try to unset it, so the random number would be changed. Also note, the unconditional (well, only implicitly conditional) last option also rerolls the random number.

## Evaluations and Calling Text Items ##

If you want to have the text be dynamic, use square brackets ` [foo] `.

If you do this, the following are attempted, in this order:

  1. IF this substitution is in alien dialog text, substitute text items with the given name.
  1. find any arithmetic operations and perform the arithmetic.
  1. substitute the value of any numeric flag with that key, or 0 if none.

There are a few points concerning text item substitutions.

Brackets do not nest for text item lookups. You cannot evaluate a text item or arithmetic to determine which text item name to search for - that's silly. Just use conditions on the text items! If that doesn't cut it, call a text item that itself calls a text item, and recurse as necessary. This process is Turing-complete, so whatever you need done, it's possible.

If there is at least one text item with that name but no text item with that name has its conditions fulfilled, that's treated as a match yielding the empty string, not a failure to match, and the search does not continue on into arithmetic or keys.

Parentheses are ignored by this language, as they are too frequently used in dialogue for anyone to want to have to think about whether they're going to cause trouble.

Alien number speech (i.e. their saying numbers determined during the conversation, not written into the script) cannot represent numbers less than 1 or more than 998 996 (i.e. nearly one million). (NOTE: If you need to cover these numbers, we can stitch it together, but ask yourself whether that's really necessary). Attempting to use larger numbers naively will usually crash the game, and when it doesn't, will result in bizarre behavior.

Example:

<pre>
greeting<br>
Greetings, captain! You have [Credits] credits.<br>
.main<br>
</pre>

Here's a somewhat more canny version of the above using text items with conditions. Note that the last text item has no condition, being implicitly conditional on having been reached.

<pre>
greeting<br>
Greetings, captain! You have [credits].<br>
.main<br>
<br>
text credits !Credits<br>
no credits<br>
.<br>
<br>
text credits =Credits=1<br>
one credit<br>
.<br>
<br>
text credits<br>
[Credits] credits<br>
.<br>
</pre>

If you call a text and the key is found but none of them have the conditions met, then it just uses the empty string (and if it was alone on its line, it moves immediately to the next line as if the line were always blank, not leaving an empty statement floating around). Only if no text items are found with that key at all, does it try to evaluate it as a numeric flag.

Text items are searched-through in order. When a text item is used, its consequences are applied immediately.

## Init items ##

Init items are conditional lists of consequences applied before searching for a greeting. They are useful for debugging dialogs, or for setting up the environment.

For example, I used this construction in an improved version of the VUX dialog.
<pre>
init +preexisting_apologies=Apologies<br>
</pre>
So I could remove options that didn't make sense for the first apology of the meeting.


## Display Directives ##

Certain commands can be given to display in a certain way. For instance, display location, or linebreaks you want on a single page. These are enclosed in curly braces, except for one that isn't really a part of this language - the percent character, used in dialog, marks the beginning and end of 'computer text'. (NOTE: I will be replacing this symbol)

Some possible display directives:
<pre>
{left} I'm Zoq {center} (Fot goes here) {right} I'm Pik<br>
{top left} My lovely captain! {bottom right} Oh, ZEX!<br>
{color=FF0000}I'm suddenly talking in red{color}back to default<br>
{(} if you need to use square brackets or curly braces for some reason, this is something like how you will show them. I'll fill it in if asked. {)}<br>
{}... and that empty pair of braces kept the '...' at the beginning of this line from being interpreted as the end of this text.<br>
</pre>

Other special directives are possible for 'dimming the lights'/vfx, or even changing who you're talking to altogether, but they haven't been specified yet. (NOTE: No actual display directives exist yet, only the '%' one. The only one I guarantee implementing soon is to strip out {} so you can start lines with periods. Don't feel constrained by what exists. This language is in development. If you need it, ask for it! )

## Example dialog ##

Here's a moderately complicated dialog using most language features.

<pre>
greeting !Rep<0  // If this race's reputation is not worse than starting, use this greeting.<br>
Hello, captain!<br>
.main // go to 'main' node<br>
<br>
greeting // if you get here, you must have negative reputation<br>
Captain, you denied us slug repellent when we needed it.<br>
DIE!<br>
.fight // You don't even get to talk to them :(<br>
<br>
<br>
node main<br>
<br>
option >computer_checked<br>
(With microphone turned off) Computer, what is my reputation with these aliens?<br>
YOUR REPUTATION WITH THESE ALIENS IS [Rep], CAPTAIN.<br>
[advice] // remember text items?<br>
.main<br>
<br>
// So we have two 'hi' options. The first is if you haven't yet helped them with their slug problem...<br>
// note that these are named so only one will be used.<br>
// also note the >said_hi so you don't say 'hi' more than once per conversation.<br>
<br>
<br>
option hi >said_hi !SLUGS_DEFEATED<br>
Why hello yourself!<br>
I'm so glad you came, captain!<br>
We've come under attack by vicious slugs!<br>
.slugs // go to the 'slugs' node if this option is taken<br>
<br>
<br>
option hi >said_hi // implicitly =SLUGS_DEFEATED due to also being a 'hi' option.<br>
Why hello yourself!<br>
It's so nice to see you again, captain<br>
especially since you helped us with our slug infestation!<br>
.main<br>
<br>
<br>
// we also have two more named options here in the main node, this time 'bye'<br>
// The exact 'bye' option depends on whether you said 'hi' in this conversation yet<br>
option bye =said_hi<br>
goodbye<br>
Return soon, captain!<br>
.done<br>
<br>
<br>
option bye // implicitly !said_hi, by virtue of also being a 'bye' option<br>
Actually, I just dropped by for a moment...<br>
Oh, how disappointing!<br>
.done<br>
<br>
<br>
// new node! It is the 'slugs' node mentioned above.<br>
node slugs<br>
<br>
option >repellent_checked<br>
(With microphone turned off) Uhh, Computer, do you know if we have some slug repellent somewhere aboard this ship?<br>
LET ME CHECK OUR INVENTORY, CAPTAIN...<br>
[checkrepellentinventory] // can be one of two possibilities, depending on whether you have it.<br>
// could have alternately made two named options<br>
.slugs<br>
<br>
option <HAVE_SLUG_REPELLENT +SLUGS_DEFEATED +Rep=3<br>
// Okay, what's all that about?<br>
// <HAVE_SLUG_REPELLENT means that you need to have slug repellent to take this option, and if you take this option, you lose it.<br>
// It's all upper case because it's a global property<br>
// +SLUGS_DEFEATED means it's recorded that the slugs have been defeated<br>
// +Rep=3 indicates that your reputation with these folks is now 3, whatever that means.<br>
// Note, 'these folks' means anyone whose dialog is based off of this dialog file<br>
// Okay, now on to the lines. First the captain, then theirs...<br>
Oh, here, have our slug repellent!<br>
Thank you for your generosity, captain!<br>
.main<br>
<br>
<br>
option =HAVE_SLUG_REPELLENT +Rep=-7<br>
Even though we have slug repellent, you can't have any!<br>
You fiend!<br>
.fight<br>
// In contrast with the above, this one requires that you have slug repellent but doesn't use it up.<br>
// also, they get mad at you, setting rep to -7.<br>
<br>
<br>
option >try_shoot<br>
Did you try shooting them?<br>
[no_work] // This calls a text item named no_work.<br>
.slugs<br>
<br>
<br>
option ~BLUE_SWITCH_SET<br>
Try hitting the big blue switch over there.<br>
Umm, okay?<br>
[no_work] // reuse same text item.<br>
.slugs<br>
// This one toggles the blue switch (whatever that is - if it does anything, this doesn't do that)<br>
// It can be selected over and over and over since we didn't put a condition on it.<br>
<br>
<br>
option<br>
I'm sorry, I'm out of ideas.<br>
That's unfortunate, but we understand.<br>
.main<br>
// Out last text option, and the least interesting. It returns you to the main node.<br>
// Note it hasn't got any conditions, which makes it safe -<br>
// you won't accidentally terminate the conversation prematurely<br>
<br>
<br>
// remember that text item call? This is what it looks like the first time...<br>
text no_work >already_took_advice<br>
We tried that.<br>
It didn't work.<br>
.<br>
<br>
<br>
text no_work // second and later times this is called in one conversation, get this.<br>
We tried that.<br>
It didn't work.<br>
Again.<br>
.<br>
<br>
text advice !Rep<0<br>
WE HAVEN'T MADE THESE ALIENS ANGRY YET. LET'S KEEP IT THAT WAY.<br>
.<br>
<br>
<br>
text advice // Meaning Rep < 0. Won't ever get shown, though, since if you've made them angry they won't ever talk to you again.<br>
EXERCISE CAUTION, CAPTAIN. SOMEHOW WE'VE MADE THESE GUYS ANGRY. BETTER WARM UP THEM WEAPONS!<br>
.<br>
<br>
text checkrepellentinventory =HAVE_SLUG_REPELLENT<br>
HMM.. IT LOOKS LIKE WE DO ACTUALLY HAVE SOME OF IT.<br>
TOO BAD YOU'LL HAVE TO EXTRACT IT FROM OUR IN-SHIP SEWER SYSTEM, THOUGH.<br>
HAVE FUN WITH THAT. I WON'T BE CAPABLE OF HELPING YOU WITH IT.<br>
(THANK GOD)<br>
.<br>
<br>
text checkrepellentinventory // implicitly !HAVE_SLUG_REPELLENT<br>
WELL, IT LOOKS LIKE WE DON'T HAVE ANY SLUG REPELLENT ABOARD OUR SHIP.<br>
DID YOU HONESTLY THINK THAT WE WOULD? COME ON NOW.<br>
INTERESTINGLY ENOUGH, MY INVESTIGATIONS SHOW THAT WE HAVE SOME NICE HOLOVIDS.<br>
ANALYSIS SHOWS THAT THEY HAVE A 100% CHANCE OF OFFENDING YOU.<br>
I THINK YOU SHOULD WATCH THEM.<br>
.<br>
</pre>