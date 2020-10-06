#include <bits/stdc++.h>
using namespace std;

int metodo;
int qnt = 5;
vector<float> constante(qnt);
vector<int> instancia(30);
string arquivoEntrada = "aquii.txt";
vector<vector<vector<long long int>>> dados(qnt);

void leDados(){
    int i,j;
    ifstream file(arquivoEntrada.c_str());
    if(!file){
        cout << "Erro ao abrir o arquivo " << arquivoEntrada << endl;
        exit(0);
    }
    file >> metodo;
    cout << metodo << endl;
    for(i = 0; i < qnt ; i++ ){
        dados[i].resize(3);
        dados[i][0].resize(30);
        dados[i][1].resize(30);
        dados[i][2].resize(30);
        file >> constante[i];
        constante[i] = 0.1*constante[i] + 3;
        cout << constante[i] << endl;
        for(j = 0; j < 30 ; j++ ){
            instancia[j] = j+1;
            file >> dados[i][0][j];
            file >> dados[i][1][j];
            file >> dados[i][2][j];
            cout << dados[i][0][j] << " " << dados[i][1][j] << dados[i][2][j] << endl;
        }
    }
    file.close();
}

int main(){
    int i,j;
    leDados();
    fstream file;
    string nomePDF = "comp.tex";
    file.open(nomePDF,ios::out);
    file << "\\documentclass[]{article}" << endl;

    file << "\\usepackage{multirow}" << endl;
    file << "\\usepackage[a4paper, bottom=1.5cm, top=1.5cm, left=0.1cm, right=0.1cm]{geometry}" << endl;

    file << "\\usepackage{longtable}" << endl;
    file << "\\usepackage{accents}" << endl;
    file << "\\begin{document}" << endl;
    for(i = 0 ; i < 30; i++) {
        file << "   \\begin{center}" << endl;
        file << "       \\begin{tabular}{ |c|c|c|c|c| }" << endl;
        file << "       \\hline" << endl;
        file << "       Instancia & Constante & Custo Final & Objetivo Obtido & Sobras \\\\" << endl;
        file << "       \\hline" << endl;
        file << "       \\multirow{5}{*}{" << instancia[i] << "}" << endl;
            for(j = 0; j < qnt ; j++ ){
                file << "       & " << constante[j] << " & " << dados[j][0][i] << " & " << dados[j][1][i] << " & " << dados[j][2][i] << "\\\\" << endl;
            }
        file << "           \\hline" << endl;
        file << "       \\end{tabular}" << endl;
        file << "   \\end{center}" << endl;
    }
    file << "\\end{document};" << endl;
    file.close();
    return 0;
}



