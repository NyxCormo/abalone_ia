#ifndef ABALONE_MOVEGENERATOR_H
#define ABALONE_MOVEGENERATOR_H
#include <vector>

#include "Board.h"
#include "Move.h"
#include "Position.h"


class MoveGenerator {
public:

    static std::vector<Move> generateMoves(const Board& board, Player player);

    static bool isLegalMove(const Board& board, Player player, const Move& move);

private:

    static void generateSingleMarbleMoves(
        const Board& board,
        Player player,
        const Position& marble,
        std::vector<Move>& moves
    );

    static void generateTwoMarbleMoves(
        const Board& board,
        Player player,
        const Position& pos,
        std::vector<Move>& moves
    );

    static void generateThreeMarbleMoves(
        const Board& board,
        Player player,
        const Position& pos,
        std::vector<Move>& moves
    );

    static bool isLegalInlineMove(const Board& board, Player player, const Move& move);
    static bool isLegalSideStepMove(const Board& board, Player player, const Move& move);

    static int countPushedMarbles(const Board& board, Player player, const Move& move);

    static std::vector<Position> getAlignedPositions(
        const Board& board,
        const Position& start,
        Direction dir,
        int maxLength
    );
};


#endif