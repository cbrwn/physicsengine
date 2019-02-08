/* =================================
 *  Base State
 *  Base class for game states
 * ================================= */
#include "basestate.h"

BaseState::BaseState(Game* game) : m_game(game) { } 
BaseState::~BaseState() { }
