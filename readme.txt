[HOW TO PLAY]

This game uses a simple verb-noun system - that is, one verb followed by one
noun, no adverbs, adjectives or specifiers. It's pretty basic. To go somewhere,
simply type the cardinal direction or its abbreviation. For example,
'northwest' or 'nw' to travel northwest.

The basic commands are: quit, look, inventory

And some handy verbs are: take, drop, examine, read, use, climb, eat, open,
close, lock, unlock

It's pretty simple to get the hang of. Have fun!

[ABOUT]

This is a simple text adventure that I made. It's my second real attempt at a
text adventure, ever. I think I deleted my first - I made it when I was
learning C. The C for one probably looks a lot like my stuff from back then, as 
I wrote it in C89.

Anyhow, when I previously tried to make a text adventure, I never really knew
where to start. For this attempt, I basically wrote a lousy lisp-like scripting
language in C and wrote the bulk of the logic in that. It's been interesting
to see how easy it is to create a (bad) version of lisp. Even so, I'd probably
do things differently in the future.

Overall, the game isn't anything special. It's short, the parser is fairly
simple (but I decided to support 'and' and ',', for some reason) and there
isn't much of a plot, but overall I think it came out okay. The funny thing
about writing in C89 is that it was pretty much smooth sailing all the way. I
guess declaring variables in the beginning of functions can catch a lot of
silly errors.

[COMPILING]

I would say this works on most compilers, but I've only managed to get it to
run properly after compiling with gcc. I tried Borland TurboC 1.5, but it
doesn't work for now.

Anyhow, *hopefully* compiling it is as simple as typing 'cc adv.c -o adv'. Make
sure that 'adv.sal' is in the same directory when you run it.

[LICENSE]

This game is licensed under the MIT license. Feel free to use and distribute
it. Check out 'license.txt' for more info.

