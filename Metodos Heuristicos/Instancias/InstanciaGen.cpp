#include <bits/stdc++.h>
using namespace std;

//int maxPeriodos = 10;
int maxQntObjetos = 7;
int minQntObjetos = 1;
int maxTamObjetos = 600;
int minTamObjetos = 150;
int maxCusto = 5;
int maxTamItens = 120;
int minTamItens = 20;
int maxQntItens = 10;
int minQntItens = 1;
int maxDemanda = 5;

string nomePDF = "DadosInstancia1.tex";

vector<int> m;
vector<vector<int>> W;
vector<vector<int>> c;
vector<int> n;
vector<vector<int>> w;
vector<vector<int>> d;
int p = 8;

void GeraValores(){
    srand((unsigned int)rand());
    int i,j;
    cout << p << endl;
    for(i=0;i<p;i++) m.push_back(minQntObjetos + rand()%maxQntObjetos);

    W.resize(p);
    cout << endl;
    for(i=0;i<p;i++){
        for(j=0;j<m[i];j++) W[i].push_back(minTamObjetos + rand()%maxTamObjetos);
    }

    c.resize(p);
    for(i=0;i<p;i++){
        for(j=0;j<m[i];j++) c[i].push_back(1 + rand()%maxCusto);
    }

    for(i=0;i<p;i++) n.push_back(minQntItens + rand()%maxQntItens);

    w.resize(p);
    for(i=0;i<p;i++){
        for(j=0;j<n[i];j++) w[i].push_back(minTamItens + rand()%maxTamItens);
    }

    d.resize(p);
    for(i=0;i<p;i++){
        for(j=0;j<n[i];j++) d[i].push_back(1 + rand()%maxDemanda);
    }
}

void ArquivoTxt(string inst){
    int i,j,k;
    fstream file;
    string nomeArquivo = "instancia" + inst + ".txt";
    file.open(nomeArquivo,ios::out);
    file << p << endl;
    for( k = 0 ; k < p ; k++ ) file << m[k] << " ";
    file << endl;
    for( k = 0 ; k < p ; k++ ){
        for( i = 0; i < m[k] ; i++ ){
            file << W[k][i] << " ";
        }
        file << endl;
    }
    for( k = 0 ; k < p ; k++ ){
        for( i = 0; i < m[k] ; i++ ){
            file << c[k][i] << " ";
        }
        file << endl;
    }
    for( k = 0 ; k < p ; k++ ) file << n[k] << " ";
    file << endl;
    for( k = 0 ; k < p ; k++ ){
        for( j = 0; j < n[k] ; j++ ){
            file << w[k][j] << " ";
        }
        file << endl;
    }
    for( k = 0 ; k < p ; k++ ){
        for( j = 0; j < n[k] ; j++ ){
            file << d[k][j] << " ";
        }
        file << endl;
    }
}
void Confere(int v){
    int i,j;
    int somatorioItens;
    int somatorioObjetos;
    while(true){
        restart:GeraValores();
        for(i=0;i<p;i++){
            somatorioItens = 0;
            somatorioObjetos = 0;
            for(j=0;j<n[i];j++){
                somatorioItens += w[i][j]*d[i][j];
            }
            for(j=0;j<m[i];j++){
                somatorioObjetos += W[i][j];
            }
            if(somatorioItens > somatorioObjetos){
                cout << endl << "Solução impossivel no periodo " << i + 1 << endl;
                cout << somatorioItens << "  -- " << somatorioObjetos << endl;
                cout << "Gerando valores novamente..." << endl;
                m.clear();
                W.clear();
                c.clear();
                n.clear();
                w.clear();
                d.clear();
                goto restart;
            }
        }
        cout << endl << "existe solução para os valores gerados..." << endl;
        ostringstream str1;
        str1 << (v+1);
        ArquivoTxt(str1.str());
        break;
    }
}

void geraPDF(){
    int i,j,k,v;
}
int main(){
    int i,j,k,v;
    fstream file;
    Confere(14);
    /*file.open(nomePDF,ios::out);
    file << "\\documentclass[]{article}" << endl << endl;
    file << "\\usepackage{multirow}" << endl;
    file << "\\usepackage[a4paper, bottom=1.5cm, top=1.5cm, left=0.1cm, right=0.1cm]{geometry}" << endl << endl;

    file << "\\usepackage{longtable}" << endl;
    file << "\\usepackage{accents}" << endl << endl;

    file << "%\\setlength{\\arrayrulewidth}{1mm}" << endl;
    file << "%\\setlength{\\tabcolsep}{18pt} " << endl;
    file << "\\renewcommand{\\arraystretch}{1.5}" << endl << endl;

    file << "\\newcommand{\\MyTilde}[1]{\\accentset{\\sim}{#1}}" << endl << endl;

    file << "%opening" << endl;
    file << "\\title{}" << endl;
    file << "\\author{}" << endl << endl;


    file << "\\begin{document}" << endl << endl;
    file << "   \\footnotesize" << endl;
    file << "   \\begin{longtable}{|c|c|rl|rrll|}" << endl;
    file << "   \\caption{{\\normalsize Dados das instâncias de testes}} \\label{tab:dadosInstancia} \\\\" << endl << endl;

    file << "   \\hline" << endl << endl;

    file << "   \\multirow{2}{*}{Inst\\^{a}ncia} & \\multirow{2}{*}{$P$} & \\multicolumn{ 2}{c|}{Objetos} & \\multicolumn{ 4}{c|}{Itens} \\\\ \\cline{ 3- 8}" << endl;
    file << "    &   & \\multicolumn{1}{l}{$m^s$} & $C_i^s$\\times$W_i^s$ & \\multicolumn{1}{l}{$n^s$} & \\multicolumn{1}{l}{$\\MyTilde{n}^s$} & $w_j^s$ & $ $ \\\\" << endl << endl;

    file << "   \\hline" << endl;
    file << "   \\endfirsthead" << endl << endl;

    file << "   \\multicolumn{8}{c}%" << endl;
    file << "   {\\tablename\\ \\thetable{} -- continuação da página anterior} \\\\" << endl;
    file << "   \\hline" << endl << endl;

    file << "   \\multirow{2}{*}{Inst\\^{a}ncia} & \\multirow{2}{*}{$P$} & \\multicolumn{ 2}{c|}{Objetos} & \\multicolumn{ 4}{c|}{Itens} \\\\ \\cline{ 3- 8}" << endl;
    file << "    &   & \\multicolumn{1}{l}{$m^s$} & $C_i^s$\\times$W_i^s$ & \\multicolumn{1}{l}{$n^s$} & \\multicolumn{1}{l}{$\\MyTilde{n}^s$} & $w_j^s$ & $ $ \\\\" << endl << endl;

    file << "   \\hline" << endl;
    file << "   \\endhead" << endl << endl;

    file << "   \\hline \\multicolumn{8}{|r|}{{Continua na próxima página}} \\\\ \\hline" << endl;
    file << "   \\endfoot" << endl << endl;

    file << "   \\hline" << endl;
    file << "   \\endlastfoot" << endl;

    //Dados das instâncias
    for(v = 0; v < 30 ; v++) {
        file << "   \\multirow{" << p << "}{*}{" << v+1 << "} & \\multirow{" << p << "}{*}{" << p << "}";
        if(v == 10) p = 8;
        if(v == 20) p = 12;
        Confere(v);
        int itensTotais;
        for (k = 0; k < p; k++) {
            file << "  &  " << m[k] << "  &  ";
            for (i = 0; i < m[k]; i++) {
                if (i + 1 == m[k]) file << c[k][i] << "(" << W[k][i] << ")";
                else file << c[k][i] << "(" << W[k][i] << ")";
            }
            itensTotais = 0;
            for (j = 0; j < n[k]; j++) {
                itensTotais += d[k][j];
            }
            file << "  &  " << itensTotais;
            file << "  &  " << n[k] << " & ";
            for (j = 0; j < n[k]; j++) {
                if (d[k][j] > 1) {
                    file << d[k][j] << "(" << w[k][j] << ")";
                } else file << w[k][j];
                if (j + 1 != n[k]) file << ", ";
            }
            file << " & \\\\ " << endl;
            if (k + 1 != p) file << "  & ";
        }
        file << "\\hline" << endl;
        m.clear();
        W.clear();
        c.clear();
        n.clear();
        w.clear();
        d.clear();
    }

    file << "   \\end{longtable}" << endl;
    file << "\\end{document};" << endl;
    file.close();*/
}

