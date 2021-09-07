#include "separation.h"

int getMaximumMaxBack (vector <double> &maxBackValues, vector <int> &s, int dimension) {

    double maximum = __DBL_MIN__;
    int index;

    for (int i = 0; i < dimension; i++) {

        bool flag = false;

        // cout << maxBackValues[i] << " " << endl;
        for (int j = 0; j < s.size(); j++) {

            if (i == s[j]) {
                flag = true;
                break;
            }

        }

        if (maxBackValues[i] > maximum && !flag) {
            maximum = maxBackValues[i];
            index = i;
        }

    }

    return index;
}

void getMaxBackValues (vector <double> &maxBackValues, vector <int> &s, double ** weight, int dimension) {

    // Calcular os valores do peso para vertices que não pertencem a S
    for (int i = 0; i < dimension; i++) {

        double maxBack = 0;
        bool flag = false;

        // Verifies if vertex belongs to S
        for (int j = 0; j < s.size(); j++) {

            if (i == s[j]) {
                flag = true;
                break;
            }

        }

        // if vertex don't belong to S
        if (!flag) {

            for (int j = 0; j < s.size(); j++) {

                if (i > s[j]) maxBack += weight[s[j]][i];
                else if (i < s[j]) maxBack += weight[i][s[j]];

            }

        }

        maxBackValues[i] = maxBack;

    }

}

double getCut (int vertex, double ** weight, int dimension) {

    double cut = 0;

    for (int i = 0; i < dimension; i++) {

        if (i > vertex) cut += weight[vertex][i];
        else if (i < vertex) cut += weight[i][vertex];

    }

    // cout << "CUT: " << cut << endl;

    return cut;

}

void updateMaxBack (vector <double> &maxBackValues, int maximumMaxBack, vector <int> &s, double ** weight, int dimension) {

    for (int i = 0; i < dimension; i++) {

        bool flag = false;

        for (int j = 0; j < s.size(); j++) {

            if (i == s[j]) {

                flag = true;
                break;

            }

        }

        if (!flag) {

            if (i > maximumMaxBack) maxBackValues[i] += weight[maximumMaxBack][i];
            else if (i < maximumMaxBack) maxBackValues[i] += weight[i][maximumMaxBack];

        }
    }
}

vector <vector <int>> MaxBack(double ** weight, int dimension) {

    // cout << "MAX BACK" << endl;

    vector <vector <int>> sets;
    vector <int> searched;

    int vertex, iter = 0;

    while (searched.size() < dimension) {

        vector <int> sMin, s;

        for (int i = 0; i < dimension; i++) {
            
            bool flag = false;

            for (int j = 0; j < searched.size(); j++) {

                if (i == searched[j]) flag = true;
                
            }

            if (!flag) {
                vertex = i;
                break;
            }

        }

        sMin.push_back (vertex);
        s = sMin;

        double cutMin = getCut (s.front(), weight, dimension);
        double cutValue = cutMin;

        vector <double> maxBackValues (dimension);
        getMaxBackValues (maxBackValues, s, weight, dimension);

        while (s.size() < dimension) {

            int maximumMaxBack = getMaximumMaxBack (maxBackValues, s, dimension);

            cutValue += 2 - 2 * maxBackValues[maximumMaxBack];
            s.push_back (maximumMaxBack);

            updateMaxBack (maxBackValues, maximumMaxBack, s, weight, dimension);

            if (cutValue < cutMin) {
                cutMin = cutValue;
                sMin = s;
            }

        }

        if (sMin.size() == dimension) break; 

        sets.push_back (sMin);

        for (int i = 0; i < sMin.size(); i++) {

            bool flag = false;

            for (int j = 0; j < searched.size(); j++) {
                if (sMin[i] == searched[j]) {
                    flag = true;
                    break;
                }
            }

            if (!flag) {
                searched.push_back (sMin[i]);
            }

        }

    }

    return sets;

}

double minCutPhase (vector <vector <int>> &V, double ** weight, int a, int dimension) {

    vector <int> A;
    A.push_back (a);

    vector <double> weightSumValues (dimension);

    while (A.size() != V.size()) {

        double maximum = __DBL_MIN__;
        int index;

        // Calcular a soma dos pesos
        for (int i = 0; i < dimension; i++) {

            bool flag = false;

            for (int j = 0; j < A.size(); j++) {
                if (i == A[j]) {
                    flag = true;
                    break;
                }
            }

            if (!flag) {

                if (i > A[0]) weightSumValues[i] += weight[A[0]][i];
                else if (i < A[0]) weightSumValues[i] += weight[i][A[0]];

            }

            // Verificar maior soma dos pesos
            // Adicionar a A
            // Calcular e retornar cut of phase

        }
    }
}

vector <vector <int>> MinCut(double ** weight, int dimension) {

    cout << "MIN CUT" << endl;

    srand (time(NULL));
    int a = rand() % dimension;

    vector <vector <int>> V, sets;

    for (int i = 0; i < dimension; i++) {

        V.push_back ({i});

    }

    while (V.size() > 1) {

        double cutOfPhase = minCutPhase (V, weight, a, dimension);

        // Fazer o merge entre s e t

    }

    return sets;

}