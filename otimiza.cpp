while(!fim)
{
    int tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
    for (int c=1; c <= tab.cor; c++)
    {
        for (int t=0; t < tam_tentativas; t++)
        {
            if (grafos_tentativas[t].passos.back() == c)
                continue;

            t_grafo_tabuleiro nova_tentativa = grafos_tentativas[t];
            nova_tentativa.passos.push_back(c);
            flood(nova_tentativa, c);
            grafos_tentativas.push_back(nova_tentativa);

            fim = ganhou(nova_tentativa.grafo[0], area);
            if (fim)
            {
                jogadas.insert(jogadas.end(), nova_tentativa.passos.begin(), nova_tentativa.passos.end());
                break;
            }
        }
        if (fim)
            break;
    }
    if (fim)
        break;


    // eliminar as tentativas que geraram as novas, vindas do passo anterior
    grafos_tentativas.erase(grafos_tentativas.begin(), grafos_tentativas.begin() + tam_tentativas);

    tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem
    if (!tam_tentativas) break;

    // avaliar grafos gerados com a heurística
    pair<int, int> melhor = make_pair(1, INT_MAX); // cor, pontuacao. Quanto menor a pontuacao, melhor
    for (t_grafo_tabuleiro t : grafos_tentativas)
    {
        int avaliacao = avalia_tabuleiro(t, area, tab.cor);

        if (avaliacao < melhor.second)
        {
            melhor.first = t.passos[0];
            melhor.second = avaliacao;
        }
    }

    // // guarda a melhor jogada
    // jogadas.pop_back();
    jogadas.push_back(melhor.first);

    // eliminar os ruins
    auto it = grafos_tentativas.begin();
    while (it != grafos_tentativas.end())
    {
        if (it->passos[0] != jogadas.back())
            it = grafos_tentativas.erase(it);

        else
        {
            it->passos.erase(it->passos.begin() + 0);
            ++it;
        }
    }

    tam_tentativas = static_cast<int>(grafos_tentativas.size()); // adicionar elementos nao vai quebrar o loop, guarda informacao para eliminar os de antes tambem

}