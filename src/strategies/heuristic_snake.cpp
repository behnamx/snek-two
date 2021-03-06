#pragma once
#include <utility>
#include <math.h>
#include <limits>
#include "../common/defs.cpp"
#include "../common/game_state.cpp"
#include "../utils/profile.cpp"
#include "strategy.cpp"
#include <math.h>


class HeuristicSnake: public Strategy {
public:
    double food_weight = 10;
    double length_weight = -0.64057;
    double free_weight = 1;
    double food_exp = 1;
    HeuristicSnake();
  HeuristicSnake(double food_weight, double food_exp, double length_weight, double free_weight);
    double scoreState(GameState gs, snake_index idx);
    Direction decideMove(GameState gs, snake_index idx);
private:
  pair<double, Direction> decideMoveR(GameState gs, snake_index idx, int depth);
};

HeuristicSnake::HeuristicSnake() {};

HeuristicSnake::HeuristicSnake(double food_weight, double food_exp, double length_weight, double free_weight) {
    this->food_exp = food_exp;
    this->food_weight = food_weight;
    this->length_weight = length_weight;
    this->free_weight = free_weight;
};


double HeuristicSnake::scoreState(GameState gs, snake_index idx) {
    //profile prof(__FUNCTION__, __LINE__);
    double score = 0;
    Snake snake = gs.getSnake(idx);
    if (!snake.isAlive()) {
        return std::numeric_limits<double>::lowest();
    }

    Point head = snake.getHead();
    vector<Path> paths = gs.bfsFood(head);

    if (paths.size()) {
        if (snake.getHealth() < paths[0].length()) {
            return std::numeric_limits<double>::lowest();
        }
        int rope = snake.getHealth() - paths[0].length();
        score += food_weight * atan(food_exp * double(rope));
    }

    int free_squares = 0; // gs.voronoi(idx);
    score += free_weight * free_squares;

    if (!free_squares) {
        return std::numeric_limits<double>::lowest();
    }

    score += free_squares * free_weight;

    score += 1.0 / snake.getSize() * length_weight;

    return score;
}


pair<double, Direction> HeuristicSnake::decideMoveR(GameState gs, snake_index idx, int depth) {
    vector<pair<double, Direction>> move_scores;
    cout << "IDX: " << idx << "\n";
    for (auto dir : DIRECTIONS) {
        GameState new_state = gs;

        map<Point,int> potential_head_ons;
        Snake snake = new_state.getSnake(idx);
        Point head = snake.getHead();

        for(int other_i = 0; other_i < new_state.getSnakes().size(); other_i++){
          Snake check_snake = new_state.getSnake(other_i);
          // add to potential head ons
          if (other_i != idx && new_state.getSnakes()[other_i].isAlive()){
            vector<Point> expand = new_state.getBoard().expand( new_state.getSnakes().at(other_i).getHead());
            for(auto point : expand){
              potential_head_ons.insert(pair<Point,int>(point,other_i));
            }
          }

          // make each other snake take a non-lethal move
          if(other_i != idx && check_snake.isAlive()){
            Direction finalDir;
            for(auto otherDir : DIRECTIONS){
              GameState other_state = new_state;
              finalDir = otherDir;
              other_state.makeMove(dir, other_i);
              other_state.cleanup();
              Snake snake = other_state.getSnake(other_i);
              if (snake.isAlive()) {
                break;
              }
            }
            new_state.makeMove(finalDir, other_i);
          }
        }

        new_state.makeMove(dir, idx);
        // cout << "head of cur: " << new_state.getSnake(idx)

        double score = 0;
        if (potential_head_ons.find(head) != potential_head_ons.end()){
          Snake enemy = new_state.getSnake(potential_head_ons.find(head)->second);
          if (enemy.getSize() >= snake.getSize()){
            score += std::numeric_limits<double>::lowest() / 8;
          }
        }

        new_state.cleanup();
        if(depth > 0){
          Snake cur_snake = new_state.getSnake(idx);
          if(cur_snake.isAlive()){
            score += decideMoveR(new_state, idx, depth-1).first;
          }
          else{
            score += std::numeric_limits<double>::lowest() / 2;
          }
        }
        else{
          score += scoreState(new_state, idx);
        }

        move_scores.push_back(make_pair(score, dir));
        cout << "score: " << score << " dir: " << dir << "\n";
    }
    vector<pair<double, Direction>>::iterator result = max_element(move_scores.begin(), move_scores.end());
    cout << "chose move " << result->second << "\n";
    return *result;
}

Direction HeuristicSnake::decideMove(GameState gs, snake_index idx) {
   cout << "IDX: " << idx << "\n";
   return decideMoveR(gs, idx, 2).second;
}
