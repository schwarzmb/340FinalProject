# CSCI340 Operating Systems Final Project
#The Smoker Problem
#Authors: Matthew Schwarz and Ricky Ramos

The Cigarette Smokers Problem focuses on the topics of concurrency and threads.

There are 3 smokers who have an infinite supply of one item (lighters, cigarette paper, and tobacco).
There is one agent (non-smoker) who chooses two items to be placed on a table.
The smoker with the item that completes the trio takes the items, makes a cigarette, and smokes.
The process then repeats a designated number of times.

Each of the three smokers and the agent are all threads
A total of 4 binary semaphores have been declared in this program

To run our program, clone our repository to a directory of your choosing
Next, cd into the project and type: make
Type: make run
You will be then prompted for how many iterations you would like to occur
Hit enter

