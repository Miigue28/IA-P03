#include "AIPlayer.h"
#include "Parchis.h"

const double masinf = 9999999999.0, menosinf = -9999999999.0;
const double gana = masinf - 1, pierde = menosinf + 1;
const int num_pieces = 3;
const int PROFUNDIDAD_MINIMAX = 4;  // Umbral maximo de profundidad para el metodo MiniMax
const int PROFUNDIDAD_ALFABETA = 6; // Umbral maximo de profundidad para la poda Alfa_Beta

bool AIPlayer::move(){
    cout << "Realizo un movimiento automatico" << endl;

    color c_piece;
    int id_piece;
    int dice;
    think(c_piece, id_piece, dice);

    cout << "Movimiento elegido: " << str(c_piece) << " " << id_piece << " " << dice << endl;

    actual->movePiece(c_piece, id_piece, dice);
    return true;
}

void AIPlayer::think(color & c_piece, int & id_piece, int & dice) const{
    
    //switch (id)
    //{
    //    case 0: thinkAleatorio(c_piece, id_piece, dice); break;
    //    case 1: thinkAleatorioMasInteligente(c_piece, id_piece, dice); break;
    //    case 2: thinkFichaMasAdelantada(c_piece, id_piece, dice); break;
    //    case 3: thinkMejorOpcion(c_piece, id_piece, dice); break;
    //}

    double valor; // Almacena el valor con el que se etiqueta el estado tras el proceso de busqueda.
    double alpha = menosinf, beta = masinf; // Cotas iniciales de la poda AlfaBeta

    switch(id){
        case 0:
            valor = alphaBetaPrunning(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, ValoracionTest);
        break;
        case 1:
            valor = alphaBetaPrunning(*actual, jugador, 0, PROFUNDIDAD_ALFABETA, c_piece, id_piece, dice, alpha, beta, Heuristica);
        break;
    }
    cout << "Valor MiniMax: " << valor << "  Accion: " << str(c_piece) << " " << id_piece << " " << dice << endl;
}

double AIPlayer::maxValue(const Parchis & state, int player, int depth, const int max_depth, color & c_piece, int & id_piece, int & dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const
{
    if (depth >= max_depth || state.gameOver())
        return heuristic(state, player);

    ParchisBros hijos = state.getChildren();

    double v = menosinf;

    for (ParchisBros::Iterator it = hijos.begin(); it != hijos.end(); ++it)
    {
        Parchis child = *it;

        if (child.getCurrentPlayerId() == player)
        {
            v = maxValue(child, player, depth+1, max_depth, c_piece, id_piece, dice, alpha, beta, heuristic);
        }
        else
        {
            v = minValue(child, player, depth+1, max_depth, c_piece, id_piece, dice, alpha, beta, heuristic);
        }

        if (v > alpha)
        {
            alpha = v;
            if (depth == 0)
            {
                c_piece = it.getMovedColor();
                id_piece = it.getMovedPieceId();
                dice = it.getMovedDiceValue();
            }
            if (alpha >= beta)
            {
                return beta;
            }
        }
    }
    return alpha;
}

double AIPlayer::minValue(const Parchis & state, int player, int depth, const int max_depth, color & c_piece, int & id_piece, int & dice, double alpha, double beta, double (*heuristic)(const Parchis &, int)) const
{
    if (depth >= max_depth || state.gameOver())
        return heuristic(state, player);

    ParchisBros hijos = state.getChildren();

    double v = masinf;

    // Iteramos sobre todos los movimientos disponibles para el jugador
    for (ParchisBros::Iterator it = hijos.begin(); it != hijos.end() && alpha < beta; ++it)
    {
        Parchis child = *it;

        // Si en el siguiente turno nos toca de nuevo mover
        if (child.getCurrentPlayerId() == player)
        {
            v = maxValue(child, player, depth+1, max_depth, c_piece, id_piece, dice, alpha, beta, heuristic);
        }
        else
        {
            v = minValue(child, player, depth+1, max_depth, c_piece, id_piece, dice, alpha, beta, heuristic);
        }

        if (beta > v)
        {
            beta = v;
            if (alpha >= beta)
            {
                return alpha;
            }
        }
    }
    return beta;
}

double AIPlayer::alphaBetaPrunning(const Parchis & state, int player, int depth, const int max_depth, color & c_piece, int & id_piece, int & dice, double alpha, double beta, double (*heuristic) (const Parchis &, int)) const
{
    return maxValue(state, player, depth, max_depth, c_piece, id_piece, dice, alpha, beta, heuristic);
}

void AIPlayer::thinkAleatorio(color & c_piece,  int & id_piece, int & dice) const
{
    // IMPLEMENTACIÓN INICIAL DEL AGENTE
    // Esta implementación realiza un movimiento aleatorio.
    // Se proporciona como ejemplo, pero se debe cambiar por una que realice un movimiento inteligente
    //como lo que se muestran al final de la función.

    // OBJETIVO: Asignar a las variables c_piece, id_piece, dice (pasadas por referencia) los valores,
    //respectivamente, de:
    // - color de ficha a mover
    // - identificador de la ficha que se va a mover
    // - valor del dado con el que se va a mover la ficha.

    // El id de mi jugador actual.
    int player = actual->getCurrentPlayerId();

    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Se obtiene el vector de dados que se pueden usar para el movimiento
    current_dices = actual->getAvailableNormalDices(player);
    // Elijo un dado de forma aleatoria.
    dice = current_dices[rand() % current_dices.size()];

    // Se obtiene el vector de fichas que se pueden mover para el dado elegido
    current_pieces = actual->getAvailablePieces(player, dice);

    // Si tengo fichas para el dado elegido muevo una al azar.
    if (current_pieces.size() > 0)
    {
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]); // get<i>(tuple<...>) me devuelve el i-ésimo
        c_piece = get<0>(current_pieces[random_id]);  // elemento de la tupla
    }
    else
    {
        // Si no tengo fichas para el dado elegido, pasa turno (la macro SKIP_TURN me permite no mover).
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
}

void AIPlayer::thinkAleatorioMasInteligente(color & c_piece,  int & id_piece, int & dice) const
{
    // El id de mi jugador actual.
    int player = actual->getCurrentPlayerId();
    // Vector que almacenará los dados que se pueden usar para el movimiento
    vector<int> current_dices;
    // Vector que almacenará los ids de las fichas que se pueden mover para el dado elegido.
    vector<tuple<color, int>> current_pieces;

    // Obtengo el vector de dados que puedo usar para el movimiento.
    // - actual->getAvailableNormalDices(player) -> solo dados disponibles en ese turno.
    // Importante : "getAvailableNormalDices" me da los dados que puedo usar en el turno actual.
    // Por ejemplo, si me tengo que contar 10 o 20 solo me saldrán los dados 10 y 20.
    // Puedo saber qué más dados tengo, aunque no los pueda usar en este turno, con:
    // - actual->getNormalDices(player) -> todos los dados
    current_dices = actual->getAvailableNormalDices(player);
    // En vez de elegir un dado al azar, miro primero cuáles tienen fichas que se puedan mover.
    vector<int> current_dices_que_pueden_mover_ficha;

    for (int i = 0; i < current_dices.size(); i++)
    {
        // Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player, current_dices[i]);
        // Si se pueden mover fichas para el dado actual, lo añado al vector de dados que pueden mover fichas.
        if (current_pieces.size() > 0)
        {
            current_dices_que_pueden_mover_ficha.push_back(current_dices[i]);
        }
    }

    // Si no tengo ningún dado que pueda mover fichas, paso turno con un dado al azar (la macro SKIP_TURN me permite no mover).
    if (current_dices_que_pueden_mover_ficha.size() == 0)
    {
        dice = current_dices[rand() % current_dices.size()];
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
    // En caso contrario, elijo un dado de forma aleatoria de entre los que pueden mover ficha.
    else
    {
        dice = current_dices_que_pueden_mover_ficha[rand() % current_dices_que_pueden_mover_ficha.size()];
        // Se obtiene el vector de fichas que se pueden mover para el dado elegido.
        current_pieces = actual->getAvailablePieces(player, dice);
        // Muevo una ficha al azar de entre las que se pueden mover.
        int random_id = rand() % current_pieces.size();
        id_piece = get<1>(current_pieces[random_id]);
        c_piece = get<0>(current_pieces[random_id]);
    }
}

void AIPlayer::thinkFichaMasAdelantada(color &c_piece, int &id_piece, int &dice) const
{
    // Elijo el dado haciendo lo mismo que el jugador anterior.
    thinkAleatorioMasInteligente(c_piece, id_piece, dice);
    // Tras llamar a esta función, ya tengo en dice el número de dado que quiero usar.
    // Ahora, en vez de mover una ficha al azar, voy a mover (o a aplicar
    // el dado especial a) la que esté más adelantada
    // (equivalentemente, la más cercana a la meta).
    int player = actual->getCurrentPlayerId();
    vector<tuple<color, int>> current_pieces = actual->getAvailablePieces(player, dice);
    int id_ficha_mas_adelantada = -1;
    color col_ficha_mas_adelantada = none;
    int min_distancia_meta = 9999;
    for (int i = 0; i < current_pieces.size(); i++)
    {
        // distanceToGoal(color, id) devuelve la distancia a la meta de la ficha [id] del color que le indique.
        color col = get<0>(current_pieces[i]);
        int id = get<1>(current_pieces[i]);
        int distancia_meta = actual->distanceToGoal(col, id);
        if (distancia_meta < min_distancia_meta)
        {
            min_distancia_meta = distancia_meta;
            id_ficha_mas_adelantada = id;
            col_ficha_mas_adelantada = col;
        }
    }
    // Si no he encontrado ninguna ficha, paso turno.
    if (id_ficha_mas_adelantada == -1)
    {
        id_piece = SKIP_TURN;
        c_piece = actual->getCurrentColor(); // Le tengo que indicar mi color actual al pasar turno.
    }
    // En caso contrario, moveré la ficha más adelantada.
    else
    {
        id_piece = id_ficha_mas_adelantada;
        c_piece = col_ficha_mas_adelantada;
    }
}

void AIPlayer::thinkMejorOpcion(color &c_piece, int &id_piece, int &dice) const
{
    // Vamos a mirar todos los posibles movimientos del jugador actual accediendo a los hijos del estado actual.
    // Cuando ya he recorrido todos los hijos, la función devuelve el estado actual. De esta forma puedo saber
    // cuándo paro de iterar.
    // Para ello, vamos a iterar sobre los hijos con la función de Parchis getChildren().
    // Esta función devuelve un objeto de la clase ParchisBros, que es una estructura iterable
    // sobre la que se pueden recorrer todos los hijos del estado sobre el que se llama.

    ParchisBros hijos = actual->getChildren();

    bool me_quedo_con_esta_accion = false;

    // La clase ParchisBros viene con un iterador muy útil y sencillo de usar.
    // Al hacer begin() accedemos al primer hijo de la rama,
    // y cada vez que hagamos ++it saltaremos al siguiente hijo.
    // Comparando con el iterador end() podemos consultar cuándo hemos terminado de visitar los hijos.
    // Voy a moverme a la casilla siempre con la que gane más energía, salvo que me encuentre con
    // algún movimiento muy interesante, como comer fichas o llegar a la meta.

    int current_power = actual->getPowerBar(this->jugador).getPower();
    int max_power = -101; // Máxima ganancia de energía.

    for (ParchisBros::Iterator it = hijos.begin(); it != hijos.end() and !me_quedo_con_esta_accion; ++it)
    {
        Parchis siguiente_hijo = *it;
        // Si en el movimiento elegido comiera ficha, llegara a la meta o ganara, me quedo con esa acción.
        // Termino el bucle en este caso.
        if (siguiente_hijo.isEatingMove() or siguiente_hijo.isGoalMove() or (siguiente_hijo.gameOver() and siguiente_hijo.getWinner() == this->jugador))
        {
            me_quedo_con_esta_accion = true;
            c_piece = it.getMovedColor();
            id_piece = it.getMovedPieceId();
            dice = it.getMovedDiceValue();
        }
        // En caso contrario, me voy quedando con el que me dé más energía.
        else
        {
            int new_power = siguiente_hijo.getPower(this->jugador);
            if(new_power - current_power > max_power)
            {
                c_piece = it.getMovedColor();
                id_piece = it.getMovedPieceId();
                dice = it.getMovedDiceValue();
                max_power = new_power - current_power;
            }
        }
    }
    // Si he encontrado una acción que me interesa, la guardo en las variables pasadas por referencia.
    // (Ya lo he hecho antes, cuando les he asignado los valores con el iterador).
}

double AIPlayer::ValoracionTest(const Parchis &estado, int jugador)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.
    int ganador = estado.getWinner();
    int oponente = (jugador+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (ganador == jugador)
    {
        return gana;
    }
    else if (ganador == oponente)
    {
        return pierde;
    }
    else
    {
        // Colores que juega mi jugador y colores del oponente
        vector<color> my_colors = estado.getPlayerColors(jugador);
        vector<color> op_colors = estado.getPlayerColors(oponente);

        // Recorro todas las fichas de mi jugador
        int puntuacion_jugador = 0;
        // Recorro colores de mi jugador.
        for (int i = 0; i < my_colors.size(); i++)
        {
            color c = my_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                // Valoro positivamente que la ficha esté en casilla segura o meta.
                if (estado.isSafePiece(c, j))
                {
                    puntuacion_jugador++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_jugador += 5;
                }
            }
        }

        // Recorro todas las fichas del oponente
        int puntuacion_oponente = 0;
        // Recorro colores del oponente.
        for (int i = 0; i < op_colors.size(); i++)
        {
            color c = op_colors[i];
            // Recorro las fichas de ese color.
            for (int j = 0; j < num_pieces; j++)
            {
                if (estado.isSafePiece(c, j))
                {
                    // Valoro negativamente que la ficha esté en casilla segura o meta.
                    puntuacion_oponente++;
                }
                else if (estado.getBoard().getPiece(c, j).get_box().type == goal)
                {
                    puntuacion_oponente += 5;
                }
            }
        }

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return puntuacion_jugador - puntuacion_oponente;
    }
}

pair<color, int> AIPlayer::getClosestPieceToGoal(const Parchis & state, color player)
{
    // Calculamos la ficha más cercana a la meta de distinto color
    pair<color, int> closest;
    int min = 100, distance;
    for (color c : state.game_colors)
    {
        for (int j = 0; j < num_pieces; j++)
        {
            distance = state.distanceToGoal(c, j);
            if (0 < distance && distance < min && c != player)
            {
                min = distance;
                closest.first = c;
                closest.second = j;
            }
        }
    }
    return closest;
}

pair<color, int> AIPlayer::getClosestPieceToPlayer(const Parchis & state, color player, int id_piece)
{
    // Calculamos la ficha más cercana al jugador de distinto color
    pair<color, int> closest;
    int min = 100, distance;
    for (color c : state.game_colors)
    {
        for (int j = 0; j < num_pieces; j++)
        {
            distance = state.distanceBoxtoBox(c, j, player, id_piece);
            if (0 < distance && distance < min && c != player && j != id_piece)
            {
                min = distance;
                closest.first = c;
                closest.second = j;
            }
        }
    }
    return closest;
}

double AIPlayer::setPowerBarScore(const Parchis & state, int player)
{
    vector<color> colors = state.getPlayerColors(player);
    bool have_other_available_dices = (state.getAvailableNormalDices(player).size() > 0);
    
    int power = state.getPower(player);
    // MOVIMIENTO RÁPIDO
    if (0 <= power && power < 50) return (power/7)*10;
    // CONCHA ROJA
    else if ((50 <= power && power < 60) || (70 <= power && power < 75)) 
    {
        bool closest_enemy = true;
        // Determinamos si la pieza más cercana es aliada
        for (int i = 0; i < colors.size(); i++)
        {
            for (int j = 0; j < num_pieces; j++)
            {
                if (state.distanceToGoal(colors[i], j) > 0)
                {
                    pair<color, int> closest = getClosestPieceToPlayer(state, colors[i], j);
                    if (closest.first == colors[(i+1) % colors.size()])
                    {
                        closest_enemy = false;
                    }
                }
            }
        }
        return closest_enemy ? 25 : 5;
    }
    // BOOM
    else if (60 <= power && power < 65) return have_other_available_dices ? 10 : -25;
    // MOVIMIENTO ULTRARÁPIDO
    else if (65 <= power && power < 70) return 100;
    // MOVIMIENTO BALA
    else if (75 <= power && power < 80) return 200;
    // CATAPUM
    else if (80 <= power && power < 85) return have_other_available_dices ? 20 : -50;
    // CONCHA AZUL
    else if (85 <= power && power < 90) 
    {
        bool closest_enemy_goal = true;
        // Determinamos si la pieza más cercana a la meta es de un color aliado
        for (int i = 0; i < colors.size(); i++)
        {
            pair<color, int> closest_to_goal = getClosestPieceToGoal(state, colors[i]);
            if (closest_to_goal.first == colors[(i+1) % colors.size()])
            {
                closest_enemy_goal = false;
            }
        }
        return closest_enemy_goal ? 50 : 5;
    }
    // BOOMBOOM
    else if (90 <= power && power < 95) return have_other_available_dices ? 30 : -100;
    // MOVIMIENTO ESTRELLA
    else if (95 <= power && power < 100) return 300;
    // CATAPUMCHIMPUM
    else return -100;
}

double AIPlayer::setScore(const Parchis & state, int player)
{
    // Colores que juega el jugador
    vector<color> colors = state.getPlayerColors(player);

    int score = 0;
    
    // Recorro las fichas del jugador
    for (color c : colors)
    {
        for (int j = 0; j < num_pieces; j++)
        {
            // Valoramos positivamente que la ficha esté en casilla segura o casi meta.
            if (state.isSafePiece(c, j))
            {
                score += 10;
            }
            else if (state.getBoard().getPiece(c, j).get_box().type == final_queue)
            {
                score += 25;
            }
            // Valoramos positivamente que la ficha esté cerca de la meta
            score += 200 - state.distanceToGoal(c, j);
        }

        // Valoramos positivamente que tengamos fichas en meta
        score += state.piecesAtGoal(c)*50;

        // Valoramos negativamente que tengamos fichas en casa
        score -= state.piecesAtHome(c)*15;

        // Valoramos negativamente que rebote mucho en la meta
        int bounces = state.getBounces(c);
        if (5 <= bounces && bounces < 10)
        {
            score -= 10;
        }
        else if (10 <= bounces && bounces < 20)
        {
            score -= 25;
        }
        else if (20 <= bounces && bounces < 30)
        {
            score -= 50;
        }
    }

    // Bonificaciones si hemos realizado la última acción
    tuple<color, int, int> last_action = state.getLastAction();
    if (get<0>(last_action) == colors[0] || get<0>(last_action) == colors[1])
    {
        pair<color, int> eaten = state.eatenPiece();
        if (eaten.first == none)
        {
            score += 0;
        }
        // Valoramos muy positivamente si la ficha comida en el último turno no fue suya
        else if (eaten.first != colors[0] && eaten.first != colors[1])
        {
            score += 100;
        }
        // Valoramos negativamente si ha comido una ficha suya
        else if (eaten.first == colors[0] || eaten.first == colors[1])
        {
            score -= 50;
        }

        int dice = state.getLastDice();
        if (dice == 6 && state.piecesAtHome(get<0>(last_action)) == 0)
        {
            score += 15;
        }
    }

    //score += setPowerBarScore(state, player);

    // Bonificaciones si se han destruido fichas rivales en el último turno
    vector<pair<color, int>> destroyed = state.piecesDestroyedLastMove();
    for (auto d : destroyed)
    {
        if (d.first == none)
        {
            score += 0;
        }
        else if (d.first != colors[0] && d.first != colors[1])
        {
            score += 50;
        }
        else if (d.first == colors[0] || d.first == colors[1])
        {
            score -= 50;
        }
    }

    return (double) score;
}

double AIPlayer::Heuristica(const Parchis & state, int player)
{
    // Heurística de prueba proporcionada para validar el funcionamiento del algoritmo de búsqueda.
    int winner = state.getWinner();
    int adversary = (player+1) % 2;

    // Si hay un ganador, devuelvo más/menos infinito, según si he ganado yo o el oponente.
    if (winner == player)
    {
        return gana;
    }
    else if (winner == adversary)
    {
        return pierde;
    }
    else
    {
        double player_score = setScore(state, player);
        double adversary_score = setScore(state, adversary);

        // Devuelvo la puntuación de mi jugador menos la puntuación del oponente.
        return player_score - adversary_score;
    }
}

