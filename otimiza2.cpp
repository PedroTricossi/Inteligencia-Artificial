while (!fim)
{
    int tam_tentativas = static_cast<int>(grafos_tentativas.size());
    #pragma omp parallel for shared(fim, grafos_tentativas, jogadas) schedule(dynamic)
    for (int c = 1; c <= tab.cor; c++)
    {
        for (int t = 0; t < tam_tentativas; t++)
        {
            if (grafos_tentativas[t].passos.back() == c)
                continue;

            t_grafo_tabuleiro nova_tentativa = grafos_tentativas[t];
            nova_tentativa.passos.push_back(c);
            flood(nova_tentativa, c);
            grafos_tentativas.push_back(nova_tentativa);

            #pragma omp critical
            {
                fim = ganhou(nova_tentativa.grafo[0], area);
                if (fim)
                {
                    jogadas.insert(jogadas.end(), nova_tentativa.passos.begin(), nova_tentativa.passos.end());
                    break;
                }
            }
        }

        if (fim)
            break;
    }

    if (fim)
        break;

    grafos_tentativas.erase(grafos_tentativas.begin(), grafos_tentativas.begin() + tam_tentativas);

    tam_tentativas = static_cast<int>(grafos_tentativas.size());
    if (!tam_tentativas)
        break;

    pair<int, int> melhor = make_pair(1, INT_MAX);
    #pragma omp parallel for shared(melhor, grafos_tentativas) schedule(dynamic)
    for (int i = 0; i < tam_tentativas; i++)
    {
        int avaliacao = avalia_tabuleiro(grafos_tentativas[i], area, tab.cor);

        #pragma omp critical
        {
            if (avaliacao < melhor.second)
            {
                melhor.first = grafos_tentativas[i].passos[0];
                melhor.second = avaliacao;
            }
        }
    }

    jogadas.push_back(melhor.first);

    auto it = grafos_tentativas.begin();
    while (it != grafos_tentativas.end())
    {
        if (it->passos[0] != jogadas.back())
        {
            it = grafos_tentativas.erase(it);
        }
        else
        {
            it->passos.erase(it->passos.begin());
            ++it;
        }
    }

    tam_tentativas = static_cast<int>(grafos_tentativas.size());
}