#include<bits/stdc++.h>
using namespace std;

int P,inst;
vector<int> m;
vector<vector<int>> W;
vector<vector<int>> c;
vector<int> n;
vector<vector<int>> w;
vector<vector<int>> d;

void leEntrada(const string &arquivoEntrada){
    int i,j,p,l;
    try{
        ifstream file(arquivoEntrada.c_str());
        if(!file){
            cerr << "Erro ao abrir o arquivo " << arquivoEntrada << endl;
            exit(0);
        }
        //Numero de periodos
        file >> P;

        //Numero de objetos disponíveis por periodo
        m.resize((unsigned int)(P+2));
        for(p = 0; p < P; p++) {
            file >> m[p];
        }
        m[P] = 0;
        //Comprimento de cada objeto de cada periodo
        W.resize((unsigned int)(P));
        for(p = 0; p < P; p++) {
            W[p].resize((unsigned int)(m[p]));
            for(i = 0 ; i < m[p] ; i++){
                file >> W[p][i];
            }
        }
        //Custo por unidade de comprimento de cada objeto por periodo
        c.resize((unsigned int)(P));
        for(p = 0; p < P; p++) {
            c[p].resize((unsigned int)(m[p]));
            for(i = 0 ; i < m[p] ; i++){
                file >> c[p][i];
            }
        }
        //Numero de tipo de itens demandados por periodo
        n.resize((unsigned int )(P));
        for(p = 0 ; p < P ; p++ ) {
            file >> n[p];
        }
        //Comprimento de cada tipo de item demandado por periodo
        w.resize((unsigned int)(P));
        for(p = 0 ; p <P ; p++ ){
            w[p].resize((unsigned int)(n[p]));
            for(j = 0; j < n[p]; j++){
                file >> w[p][j];
            }
        }
        //Demanda de cada tipo de item por periodo
        d.resize((unsigned int)(P));
        for(p = 0; p < P; p++){
            d[p].resize((unsigned int)(n[p]));
            for(j = 0 ; j < n[p] ; j++){
                file >> d[p][j];
            }
        }
    }
    catch(exception &e){
        cout << "Error: " << e.what() << endl;
    }
}
void imprimeEntrada(){
    int p,i,j;
    cout << "P = "  << P << ";" << endl;
    cout << "m = [";
    for(p = 0 ; p < P ; p++){
        cout << m[p];
        if(p!=P) cout << ", ";
    }
    cout << "];" << endl;

    cout << "W = [";
    for(p = 0 ; p < P ; p++){
        cout << "[";
        for(i = 0 ; i < m[p] ; i++){
            cout << W[p][i];
            if(i!=m[p]) cout << ", ";
        }
        cout << "]";
        if(p!=P) cout << "," << endl << "     ";
    }
    cout << "];" << endl;

    cout << "c = [";
    for(p = 0; p < P ; p++){
        cout << "[";
        for(i = 0 ; i < m[p] ; i++){
            cout << c[p][i];
            if(i!=m[p]) cout << ", ";
        }
        cout << "]";
        if(p!=P) cout << "," << endl << "     ";
    }
    cout << "];" << endl;
    cout << "n = [";
    for(p = 0 ; p < P ; p++){
        cout << n[p];
        if(p!=P) cout << ", ";
    }
    cout << "];" << endl;

    cout << "w = [";
    for(p = 0 ; p < P ; p++){
        cout << "[";
        for(j = 0 ; j < n[p] ; j++){
            cout << w[p][j];
            if(j!=n[p]) cout << ", ";
        }
        cout << "]";
        if(p!=P) cout << "," << endl << "     ";
    }
    cout << "];" << endl;

    cout << "d = [";
    for(p = 0 ; p < P ; p++){
        cout << "[";
        for(j = 0 ; j < n[p] ; j++){
            cout << d[p][j];
            if(j!=n[p]) cout << ", ";
        }
        cout << "]";
        if(p!=P) cout << "," << endl << "     ";
    }
    cout << "];" << endl;
}

void leArgumentos(int argc, char** argv) {

    for(int i = 1 ; i < argc ; i++) {
        if (string(argv[i]) == "-i") {
            inst = atoi(argv[i + 1]);
            continue;
        }
    }
}
int main(){
    int i,j,k,v;
    fstream file;
    string nomePDF = "esseaqui.tex";
    file.open(nomePDF,ios::out);
    file << "\\documentclass[]{article}" << endl << endl;
    file << "\\usepackage{multirow}" << endl;
    file << "\\usepackage[a4paper, bottom=2cm, top=1.5cm, left=0.1cm, right=0.1cm]{geometry}" << endl << endl;

    file << "\\usepackage{longtable}" << endl;
    file << "\\usepackage{accents}" << endl << endl;

    file << "%\\setlength{\\arrayrulewidth}{1mm}" << endl;
    file << "%\\setlength{\\tabcolsep}{18pt} " << endl;
    file << "\\renewcommand{\\arraystretch}{1.5}" << endl << endl;

    file << "\\newcommand{\\MyTilde}[1]{\\accentset{\\sim}{#1}}" << endl << endl;


    file << "\\begin{document}" << endl << endl;
    file << "   \\footnotesize" << endl;
    file << "   \\begin{longtable}{|c|c|rl|rrll|}" << endl;
    file << "   \\caption{{\\normalsize Dados das instâncias de testes}} \\label{tab:dadosInstancia} \\\\" << endl << endl;

    file << "   \\hline" << endl << endl;

    file << "   \\multirow{2}{*}{Inst\\^{a}ncia} & \\multirow{2}{*}{$P$} & \\multicolumn{ 2}{c|}{Objetos} & \\multicolumn{ 4}{c|}{Itens} \\\\ \\cline{ 3- 8}" << endl;
    file << "    &   & \\multicolumn{1}{l}{$m^s$} & $C_i^s$\\$(W_i^s)$ & \\multicolumn{1}{l}{$n^s$} & \\multicolumn{1}{l}{$\\MyTilde{n}^s$} & $w_j^s$ & $ $ \\\\" << endl << endl;

    file << "   \\hline" << endl;
    file << "   \\endfirsthead" << endl << endl;

    file << "   \\multicolumn{8}{c}%" << endl;
    file << "   {\\tablename\\ \\thetable{} -- continuação da página anterior} \\\\" << endl;
    file << "   \\hline" << endl << endl;

    file << "   \\multirow{2}{*}{Inst\\^{a}ncia} & \\multirow{2}{*}{$P$} & \\multicolumn{ 2}{c|}{Objetos} & \\multicolumn{ 4}{c|}{Itens} \\\\ \\cline{ 3- 8}" << endl;
    file << "    &   & \\multicolumn{1}{l}{$m^s$} & $C_i^s$\\$(W_i^s)$ & \\multicolumn{1}{l}{$n^s$} & \\multicolumn{1}{l}{$\\MyTilde{n}^s$} & $w_j^s$ & $ $ \\\\" << endl << endl;

    file << "   \\hline" << endl;
    file << "   \\endhead" << endl << endl;

    file << "   \\hline \\multicolumn{8}{|r|}{{Continua na próxima página}} \\\\ \\hline" << endl;
    file << "   \\endfoot" << endl << endl;

    file << "   \\hline" << endl;
    file << "   \\endlastfoot" << endl;
    //Dados das instâncias
    for(v = 0; v < 30 ; v++) {
        string nomeArquivo = "instancia";
        nomeArquivo.append(to_string(v+1));
        nomeArquivo.append(".txt");
        //cout << nomeArquivo << endl;
        leEntrada(nomeArquivo);
        imprimeEntrada();
        file << "   \\multirow{" << P << "}{*}{" << v+1 << "} & \\multirow{" << P << "}{*}{" << P << "}";
        int itensTotais;
        for (k = 0; k < P; k++) {
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
            if (k + 1 != P) file << "  & ";
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
    file.close();
}