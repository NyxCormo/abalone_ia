//
// Created by nyx on 3/14/26.
//

#include "MoveGenerator.h"

#include <algorithm>
#include <unordered_set>

std::vector<Move> MoveGenerator::generateMoves(const Board& board, Player player) {
    std::vector<Move> moves;

    // Récupérer toutes les positions des billes du joueur
    std::vector<Position> marblePositions = board.getMarblePositions(player);

    // Utiliser un set pour éviter les doublons
    std::unordered_set<Position> processed;

    for (const Position& pos : marblePositions) {
        if (processed.count(pos)) continue;
        processed.insert(pos);

        generateSingleMarbleMoves(board, player, pos, moves);
        generateTwoMarbleMoves(board, player, pos, moves);
        generateThreeMarbleMoves(board, player, pos, moves);
    }

    std::unordered_set<std::string> seen;
    std::vector<Move> uniqueMoves;

    for (const Move& move : moves) {
        std::string key = move.toString();
        if (seen.find(key) == seen.end()) {
            seen.insert(key);
            uniqueMoves.push_back(move);
        }
    }

    return uniqueMoves;
}

void MoveGenerator::generateSingleMarbleMoves(
    const Board& board,
    Player player,
    const Position& pos,
    std::vector<Move>& moves
    ) {
    for (Direction dir : ALL_DIRECTIONS) {
        Move move(pos, dir);

        if (isLegalMove(board, player, move)) {
            moves.push_back(move);
        }
    }
}

void MoveGenerator::generateTwoMarbleMoves(
    const Board& board,
    Player player,
    const Position& pos,
    std::vector<Move>& moves
) {
    for (Direction alignDir : ALL_DIRECTIONS) {
        Position neighbor = pos.neighbor(alignDir);

        if (!board.hasMarble(neighbor, player)) {
            continue;
        }

        std::vector<Position> marbles = {pos, neighbor};

        for (Direction moveDir : ALL_DIRECTIONS) {
            Move move(marbles, moveDir);

            if (isLegalMove(board, player, move)) {
                moves.push_back(move);
            }
        }
    }
}

void MoveGenerator::generateThreeMarbleMoves(
    const Board& board,
    Player player,
    const Position& pos,
    std::vector<Move>& moves
) {
    for (Direction alignDir : ALL_DIRECTIONS) {
        Position neighbor1 = pos.neighbor(alignDir);
        Position neighbor2 = neighbor1.neighbor(alignDir);

        if (!board.hasMarble(neighbor1, player) ||
            !board.hasMarble(neighbor2, player)) {
            continue;
        }

        std::vector<Position> marbles = {pos, neighbor1, neighbor2};

        for (Direction moveDir : ALL_DIRECTIONS) {
            Move move(marbles, moveDir);

            if (isLegalMove(board, player, move)) {
                moves.push_back(move);
            }
        }
    }
}

bool MoveGenerator::isLegalMove(const Board& board, Player player, const Move& move) {
    if (move.isInline()) {
        return isLegalInlineMove(board, player, move);
    } else {
        return isLegalSideStepMove(board, player, move);
    }
}

bool MoveGenerator::isLegalInlineMove(const Board& board, Player player, const Move& move) {
    Direction dir = move.direction();
    const auto& marbles = move.marbles();

    Position front = marbles[0];
    for (const Position& marble : marbles) {
        Position behind = marble.neighbor(opposite(dir));
        bool isBehind = std::find(marbles.begin(), marbles.end(), behind) != marbles.end();

        if (!isBehind) {
            front = marble;
            break;
        }
    }

    Position target = front.neighbor(dir);

    if (!target.isValid()) {
        return false;
    }

    if (board.isEmpty(target)) {
        return true;
    }

    if (board.hasMarble(target, player)) {
        return false;
    }


    int opponentCount = 0;
    Position checkPos = target;

    while (checkPos.isValid() && board.hasMarble(checkPos, opponent(player))) {
        opponentCount++;
        checkPos = checkPos.neighbor(dir);
    }

    int ourCount = move.marbleCount();

    if (ourCount <= opponentCount) {
        return false;
    }

    if (opponentCount > 2) {
        return false;
    }

    if (checkPos.isValid() && !board.isEmpty(checkPos)) {
        return false;
    }

    return true;
}

bool MoveGenerator::isLegalSideStepMove(const Board& board, Player player, const Move& move) {
    auto destinations = move.destinations();

    for (const Position& dest : destinations) {
        if (!dest.isValid()) {
            return false;
        }

        if (!board.isEmpty(dest)) {
            return false;
        }
    }

    return true;
}

int MoveGenerator::countPushedMarbles(const Board &board, Player player, const Move &move) {
    if (!move.isInline()) {
        return 0;
    }

    Direction dir = move.direction();
    const auto& marbles = move.marbles();

    Position front = marbles[0];
    for (const Position& marble : marbles) {
        Position behind = marble.neighbor(opposite(dir));
        bool isBehind = std::find(marbles.begin(), marbles.end(), behind) != marbles.end();

        if (!isBehind) {
            front = marble;
            break;
        }
    }

    Position target = front.neighbor(dir);
    int count = 0;

    while (target.isValid() && board.hasMarble(target, opponent(player))) {
        count++;
        target = target.neighbor(dir);
    }

    return count;
}

std::vector<Position> MoveGenerator::getAlignedPositions(
    const Board &board,
    const Position &start,
    Direction dir,
    int maxLength
) {
    std::vector<Position> positions;
    Position current = start;

    for (int i = 0; i < maxLength && current.isValid(); i++) {
        positions.push_back(current);
        current = current.neighbor(dir);
    }

    return positions;
}
