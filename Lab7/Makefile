dev:
	@echo "Nampally Pranav"
	@echo "190010026@iitdh.ac.in"

random: 
	gcc random.c -o random
	./random > random.dat
	gnuplot random.p

optimal:
	gcc optimal.c -o optimal
	./optimal > optimal.dat
	gnuplot optimal.p

lru:
	gcc lru.c -o lru
	./lru > lru.dat
	gnuplot lru.p

clock:
	gcc clock.c -o clock
	./clock > clock.dat
	gnuplot clock.p

clean:
	rm -f random   
	rm -f optimal
	rm -f lru
	rm -f clock
