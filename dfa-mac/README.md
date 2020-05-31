Running through command line:

1. Git clone and build the repo

    git clone https://github.com/sashidhar-jakkamsetti/uci.cs295.git
    cd dfa/
    make

2. Start the Secure DFA monitor in a new terminal

    ./dfa.o

3. Start the untrusted syrungePump program in another terminal

    ./syringePumpSim

4. Type in size of the bolus (in uL)

    500

5. Type in "+" to PUSH that size bolus

    +

6. Type in "-" to PULL that size bolus

    -

7. Type in "q" to terminate
    
    q
