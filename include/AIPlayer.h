#ifndef __AI_PLAYER_H__
#define __AI_PLAYER_H__

#include "Attributes.h"
#include "Player.h"

class AIPlayer : public Player
{
protected:
    // Id identificativo del jugador
    const int id;

public:
    /**
     * @brief Constructor de un objeto AIPlayer
     *
     * @param name Nombre del jugador
     */
    inline AIPlayer(const string &name) : Player(name), id(1){};

    /**
     * @brief Constructor de un objeto AIPlayer
     *
     * @param name Nombre  del jugador
     * @param id Id del jugador
     */
    inline AIPlayer(const string &name, const int id) : Player(name), id(id){};

    /**
     * @brief Función que percibe el el parchís y al jugador actual.
     * Asigna el tablero en actual y el id del jugador.
     *
     * @param p Instancia Parchis que se quiere percibir
     */
    inline virtual void perceive(Parchis &p) { Player::perceive(p); }

    /**
     * @brief Función abstracta que define el movimiento devuelto por el jugador.
     * Llama a la función movePiece con el valor asignado a los parámetros pasados
     * por referencia.
     *
     * @return true
     * @return false
     */
    virtual bool move();

    /**
     * @brief Función que se encarga de decidir el mejor movimiento posible a
     * partir del estado actual del tablero. Asigna a las variables pasadas por
     * referencia el valor de color de ficha, id de ficha y dado del mejor movimiento.
     *
     * @param c_piece Color de la ficha
     * @param id_piece Id de la ficha
     * @param dice Número de dado
     */
    virtual void think(color & c_piece, int & id_piece, int & dice) const;

    virtual void thinkAleatorio(color &c_piece, int &id_piece, int &dice) const;

    virtual void thinkAleatorioMasInteligente(color &c_piece, int &id_piece, int &dice) const;

    virtual void thinkFichaMasAdelantada(color &c_piece, int &id_piece, int &dice) const;

    virtual void thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const;

    /**
     * @brief Método que determina si el player es inteligente (decide el mejor movimiento)
     * o no. True para AIPlayer.
     *
     * @return true
     * @return false
     */
    inline virtual bool canThink() const { return true; }

    /**
     * @brief Heurística de prueba para validar el algoritmo de búsqueda.
     *
     * @param estado Instancia de Parchis con el estado actual de la partida.
     * @param jugador Id del jugador actual (0 o 1)
     * @return double
     */
    static double ValoracionTest(const Parchis &estado, int jugador);

    /**
     * @brief Heurística01
     *
     * @param estado Instancia de Parchis con el estado actual de la partida.
     * @param jugador Id del jugador actual (0 o 1)
     * @return double
     */
    static double Heuristic(const Parchis &estado, int jugador);

    static pair<color, int> getClosestPieceToGoal(const Parchis & state, color player);

    static pair<color, int> getClosestPieceToPlayer(const Parchis & state, color player, int id_piece);

    static double setScore(const Parchis &estado, int jugador);

    static double setColorScore(const Parchis &estado, color jugador);

    static double setPowerBarScore(const Parchis & state, int player);

    virtual double alphaBetaPrunning(const Parchis &state, int player, int depth, const int max_depth, color &c_piece, int &id_piece, int &dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const;

    virtual double maxValue(const Parchis &state, int player, int depth, const int max_depth, color &c_piece, int &id_piece, int &dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const;

    virtual double minValue(const Parchis &state, int player, int depth, const int max_depth, color &c_piece, int &id_piece, int &dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const;
};
#endif