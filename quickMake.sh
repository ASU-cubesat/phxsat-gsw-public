rm csp-client

make clean; make; clear;

touch output1.out

gnome-terminal --title="OBC Output" --geometry 88x55+900+0 -e "tail -f output1.out"

gnome-terminal --title="Command Terminal" --geometry 88x55+0+0 -e "./csp-client" 
