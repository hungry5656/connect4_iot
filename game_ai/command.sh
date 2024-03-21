python3 main.py -p1 minimaxAI -p2 randomAI -limit_players -1 -seed 4
python3 main.py -p2 minimaxAI -p1 randomAI -limit_players -1 -seed 3
python3 main.py -p1 minimaxAI -p2 stupidAI -limit_players -1 -seed 2
python3 main.py -p2 minimaxAI -p1 stupidAI -limit_players -1 -seed 1
python3 main.py -p1 alphaBetaAI -p2 monteCarloAI -limit_players 2 -seed 1
python3 main.py -p1 alphaBetaAI -p2 monteCarloAI -limit_players 2 -seed 3
python3 main.py -p2 alphaBetaAI -p1 monteCarloAI -limit_players 2 -seed 1
python3 main.py -p2 alphaBetaAI -p1 monteCarloAI -limit_players 2 -seed 2


# a=10
# while [ $a -lt 10 ]
# do
#    python3 main.py -p1 alphaBetaAI -p2 monteCarloAI -limit_players 2 -seed $a
#    a=`expr $a + 1`
# done

# python3 main.py -p1 alphaBetaAI -p2 monteCarloAI -limit_players -1 -seed 8

# python3 main.py -p1 monteCarloAI -p2 alphaBetaAI -limit_players -1 -seed 99

# python3 main.py -p1 monteCarloAI -p2 minimaxAI -limit_players -1

# python3 main.py -p1 human -p2 minimaxAI -limit_players -1

# python3 main.py -p1 human -p2 minimaxAI -limit_players -1

# python3 main.py -p1 human -p2 monteCarloAI -limit_players 2