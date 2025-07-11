
tlc
===

tiny librus (synergia) client.

what?
---

Librus Synergia is a Polish electronic gradebook.
This is a client for it written in C99 using an unofficial and undocumented API.

the only dependencies are:

- libcurl
- jansson
- uthash (included)

how?
---

tlc was made with being scriptable in mind, it's hacking friendly in a way that you can use all your favorite shell tools to do certain actions (I personally recommend using less, if used as a client), at it's own it does only one thing - fetch and parse data from Librus API

You need to change LOGIN and PASS in config.h to your login and password.

    note: this isn't really safe, I'm thinking about GPG integration (like neomutt does for passwords), you should only run the program in a trusted and private (that is not a shared system/pubnix) environment.

I also imagine it would be quite easy to embed into other C/C++ programs with some minimal tweaking.

to install it run make install

so you can get only 6 newest grades by doing:

tlc -g | tail -6

or get only your maths grades by doing:

tlc -g | grep Matematyka

etc.

usage
---
-g: grades
-a: attendance
-l: lucky number
-n: announcements
-w: warnings/notes.
-t: tests, homework and exams.
-c: calendar with an optional parameter of YY-MM-DD as a start of the week (has to be monday)
-h: help

the flags can not be chained together.

contributing
---
if you find it even sort of useful feedback would be nice.

problems
---
This is by all means sort of janky, rerunning the program often solves any issues.

-a flag is pretty slow (~1msg per 0.5s) because of bad design choices.

this is a prototype made in couple days, I consider it usable, but YMMV.

todo
---

- add timetables/calendar (working on it) (done).
- write a client for Plan 9.
- implement a more secure way to store passwords.
- fix issue with it not working on GNU-based systems. (only MUSL-based linux tested so far) (done)

license
---
ISC, thid-party code (libraries) differs in licensing, please consult the source files.
