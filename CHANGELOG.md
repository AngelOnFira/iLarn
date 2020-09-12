# What's new in the alpha/beta version?

The current alpha version is 0.25.

    - Features
        - COLOR. [Here's how to configure it](http://roguelike-palm.sourceforge.net/color.php).
        - The source now compiles with SDK 3.5. This caused some exciting new bugs at first, which I think I have found and fixed all of, but if any remain they'll be the sort that causes Fatal Error, so, er, keep an eye out. 
    - Bug fixes
        - Changing the font does not take time now.
        - A four-button action, even cancel, takes time AFTER you answer the popup. This fixes: you getting pushed onto a monster, when you fail to open door; monster on the other side of door attacks you, when you fail to open door.
        - The Chest actions (open, take, ignore) have been reordered. See [survey](http://roguelike-palm.sourceforge.net/survey/5.html) on another reordering question.
        - Structures that are destroyed by fireball don't turn into "," now.
        - Stats are updated promptly when you pick up items that affect Ac or Wc.
        - Demonlords are now actually invisible without Eye of Larn (like they're really supposed to be, but haven't been since, hmmm, August). I play-tested it. It's not so bad. Demonlords start on level 11 and a fixed number of new ones are created each time you enter the level, so I'd suggest that you don't backtrack until you get the Eye. 
    - Bugs that I don't quite remember whether they're fixed yet or not:
    "monster 'letter' is left behind after Teleport Away or after stealing"
    "Known" bugs that I haven't been able to fix yet:
    - Rarely there's a bad volcano level, where, if you climb up/down, it crashes! (I will fix this as soon as I can reproduce it enough to figure out what the heck the problem is. This may take quite a while.)
    - New known bugs
    - The "Invert" reverse-video option has occasional white splotches at bottom. 

Features and bugfixes in beta version 0.22:

    - Only one score listed per clone. [I haven't tested this for winners yet.]
    - Difficulty is displayed (read-only) in Preferences.
    - Your name can be edited in Preferences (but is only displayed for winners).
    - Expensive purchase (e.g. sunsword) overflow bug fixed.
    - Pulverization scroll's timing bug fixed.
    - One of the "exploitable features" of the game has been made only finitely exploitable.
    - footnote: I examined behavior of the monster detection potion, which had been reported as odd. Ularn behaves the same way, it turns out, so I am demoting this from "possible bug" to "oddity of game reality". 

These features were added in beta 0.21:

    - "Clone Me": You can save a clone of your character (minus plot tokens).
    - Fixed a tax-payment-inheritance bug.
    - High score list can be viewed any time. 

What's new in the stable version?

The "stable" (== found on PalmGear) version of iLarn is: 0.20.

0.20 included these bug fixes:

    - Decreased frequency of bottomless trap doors, for difficulty = 0.
    - Fixed a tax-payment bug (encountered after winning a game).
    