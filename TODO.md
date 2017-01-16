# New features to be implemented

This is a list of things to be implemented, wich is presented in a general list for the features applicables to the whole application, and a section for each window in the application.

## General

* [DONE] Calculate stats.
* [DONE] Create a c file for each window.

## Stop Watch Window

This window is intended to be the main window, the first to be called, in this window the times will be measured and displayed.

* Modify display to under_60 seconds depending on average time.
* Decrease presicion at lower times.
* [DONE] Improve stats displayed at main screen.
* [DONE] Add stats resume in the bottom of the screen.
* [DONE] Merge solves and average layers.
* [DONE] Add nice fonts for display time.

## Select Cube Window

This is the window to select the Stop Watch mode: F2L, OLL, PLL and FULL. Each mode is intended to measure partial times and let the user know where to improve.

* Distinguish between rubik, piramyx and magaminx.
* Add solving variations, like binforded, one hand, feet.
* Add other cubes types and dimensions.
* Add mode selection, and application configuration.
* Add configmation and close the window after some time (200ms).

## Stats Window

This is the window to view the user cube stats depending on the current cube size selected.

* Add icons to each stat.
* Add an option to view all stats.
* Calculate world rank position depending on average, query from WCA.

## Delete Stats Window

This is the window to delete user stats from one cube size or all the cubes.

* Calculate the right Y coordinate to vertically center the text.
* Add confirmation and close the window after some time (200ms).
