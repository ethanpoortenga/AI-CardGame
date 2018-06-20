# AI-CardGame

The objective of this project is to create a reinforcement learning based AI that will learn from the AI who's logic I've hard coded and eventually overtake it in terms of game skill.

The structure of the neural network is as follows:
- Input Neurons
 - 40 Neurons: cards in the players hand (0 = card is not present in hand, 1 = card is present in hand)
 - 40 Neurons: cards in the trick (0 = card is not present in the trick, 1 = card is present in the trick)
 - 40 Neurons: cards already played (0 = card has not been played, 1 = card has been played)
- Hidden Layer
 - 120(Total Input Neurons) * (2/3) = 80 Hidden Neurons (as a starting point)
- Output Layer
 - 6 Neurons: for each card in the players hand (with weights based on which card is ideal to play)

After I get both of them working I'll post some stastics about the outcomes



Description of the Card Game: Smear

This version of the game is played with 4 people and 2 teams (every other person). The 6,7,8s are taken out of the deck for the sake of having more relevant cards, resulting in a deck size of 40. The game is played until one team has at least 11 point - at which point the team with 11+ points wins. Each player has a hand size of 6 to begin the hand, and plays one card each trick. The players is allowed to see their respective hands and the cards played thus far in the trick. The card that wins the trick is decided by the cards' values, taking into account trump suit. For example, if the suit led in the trick was hearts and the trumpsuit is spades, the hierarchy of cards to win the trick would be:
- A♠,K♠,Q♠,J♠,10♠,9♠,5♠,4♠,3♠,2♠,A♥,K♥,Q♥,J♥,10♥,9♥,5♥,4♥,3♥,2♥

The objective of the game is to get as many 'points' as possible in each hand, and also to cover the 'bet' you make in the beginning of the hand. When the cards are dealt at the beginning of the hand (the role of the 'dealer' moves clockwise through the players after each hand), each player (in a cloockwise fashion) will make a 'bet' as to how many points they believe team can take in during the hand. The minimum bet is 2 and the maximum bet is 4. After each player (moving clockwise) has decided whether to bet, the one who has bet the highest amount will pick trumpsuit for the hand and lead out. If nobody has bet by the time the betting reaches the dealer, the dealer is "screwed" and must bet 2 points (and gets to pick trumpsuit for the hand). If the team that bet at the beginning of the hand covers the amount of points they bet, each team is awarded the amount of points they took in during the hand. If the team that bets fails to cover their bet, they got 'set' and the amount of points they bet is subtracted from their score with the other team still being awarded the points they scored.
The points are awarded for:
1. Playing the lowest card of trump suit
2. Playing the highest card of trump suit
3. Taking in the jack of trump suit (if it was dealt)
4. Taking in the most 'game' - which is determined though the values of the cards taken in by each team in the trick, with whichever team commanding the majority of the "game points" being awarded the game point
 - Taking in a 10 is worth 10 game points
 - Taking in a Ace is worth 4 game points
 - Taking in a King is worth 3 game points
 - Taking in a Queen is worth 2 game points
 - Taking in a Jack is worth 1 game points 
 
During a trick, the person who either bet (for the first trick of the hand) or won the previous trick (for all subsequent tricks) plays the first card. The suit of the card they played is considered the 'led suit' for the trick, and all other players must follow suit if possible, with the exception that a card of trump suit can be played at any point. If a player violates the rule of following the led suit (except playing trump suit), their teams points are reduced by 4 and the hand is ended.
