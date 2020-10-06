#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

#include "Util.h"

#define CPLEX_GET(X) DOUBLE_TO_INT(cplex.getValue(X))
#define CPLEX_GETD(X) cplex.getValue(X)
#define DIR "./Instancias/"
#define INSTANCIA "instancia0.txt"


using namespace std;

typedef IloArray<IloNumVarArray> NumVarMatrix2D;
typedef IloArray<NumVarMatrix2D> NumVarMatrix3D;
typedef IloArray<IloNumArray>    FloatMatrix;
typedef IloArray<IloExprArray>   ExprMatrix;
typedef unsigned long long ullong;

void leEntrada(const string &arquivoEntrada);
void resolveModelo();
void leArgumentos(int argc, char** argv);
void setParametrosCplex(IloCplex &cplex);
void resolveModelo();
void imprimeEntrada();

/*********************************************************/
/*
 * Variáveis (dados) de entrada
 */
int numPeriodos; //Numero de Periodos
vector<int> m; //Numero de objetos disponíveis por periodo
vector<vector<float>> W; //Comprimento de cada objeto de cada periodo
vector<vector<float>> c; //Custo de cada objeto de cada periodo
vector<int> n; //Numero de tipo de itens demandados em cada periodo
vector<vector<float>> w; //Comprimento de cada tipo de item demadado por periodo
vector<vector<int>> d; //Demanda de cada tipo de item por periodo
vector<vector<int>> barC; //custo de cada objeto e de cada sobra por periodo
vector<int> barM; // numero de objetos + sobras disponíveis por periodo
/*********************************************************/

string nomeInstancia;
string nomeArquivoInstancia;
int tempoLimiteCplex;
bool imprimirLogCplex;
bool setLimiteThreadsCplex;
int numThreadsCplex;
bool setLimiteMemoriaCplex;
int maxMemoriaCplex;
int wMin = INT_MAX; //Objeto de menor tamanho
int hatW = 0; //Objeto de maior tamanho
int hatN = 0; //Maior quantidade de itens demandados
int numObjetosTotais; //Quantidade total de objetos disponíveis

int main(int argc, char** argv) {
    std::cout.imbue(std::locale(std::cout.getloc(), new DecimalSeparator<char>(',')));
    auto tempoInicio = MyClock();

    leArgumentos(argc, argv);

    leEntrada(nomeInstancia);
    //imprimeEntrada();

    resolveModelo();
    imprimeEntrada();
    cout << "Tempo Total Gasto: " << setprecision(5) << fixed << (MyClock()-tempoInicio)/CLOCKS_PER_SEC << endl << endl;

    return 0;
}

void leArgumentos(int argc, char** argv) {
    tempoLimiteCplex = 3600*4; //4 horas
    imprimirLogCplex = true;
    setLimiteMemoriaCplex = false;
    setLimiteThreadsCplex = false;

    if(argc == 1) {
        nomeInstancia = string(DIR) + string(INSTANCIA);
        tempoLimiteCplex = 3600*4;
        imprimirLogCplex = true;
        setLimiteMemoriaCplex = true;
        maxMemoriaCplex =3000;
        setLimiteThreadsCplex = false;
    }

    for(int i = 1 ; i < argc ; i++) {
        if (string(argv[i]) == "-i") {
            nomeInstancia = string(argv[i + 1]);
            continue;
        }
        if (string(argv[i]) == "-tl") {
            tempoLimiteCplex = atoi(argv[i+1]);
            continue;
        }
        if (string(argv[i]) == "-logCplex") {
            imprimirLogCplex = true;
            continue;
        }
        if(string(argv[i]) == "-mem"){
            setLimiteMemoriaCplex = true;
            maxMemoriaCplex = atoi(argv[i+1]);
            continue;
        }
        if(string(argv[i]) == "-threads"){
            setLimiteThreadsCplex = true;
            numThreadsCplex = atoi(argv[i+1]);
            continue;
        }
    }

    //separando o nome da instancia
    string::size_type loc  = nomeInstancia.find_last_of( "/", nomeInstancia.size() );
    string::size_type loc2 = nomeInstancia.find_last_of( ".", nomeInstancia.size() );
    if( !((loc != string::npos ) && (loc2 != string::npos)) ){
        cout << "ERRO!" << endl;
        exit(0);
    }
    nomeArquivoInstancia = string("");
    nomeArquivoInstancia.append(nomeInstancia, loc+1, loc2-loc-1);

    cout << nomeInstancia << endl;
    cout << "Tempo Limite Cplex: " << tempoLimiteCplex << endl;
    cout << "Imprimir Log Cplex? " << std::boolalpha << imprimirLogCplex << endl;
}

void leEntrada(const string &arquivoEntrada){
    int i,j,p,l;
    try{
        ifstream file(arquivoEntrada.c_str());
        if(!file){
            cerr << "Erro ao abrir o arquivo " << arquivoEntrada << endl;
            exit(0);
        }
        //Numero de periodos
        file >> numPeriodos;

        //Numero de objetos disponíveis por periodo
        m.resize((unsigned int)(numPeriodos+2));
        for(p = 1; p <= numPeriodos; p++) {
            file >> m[p];
        }
        m[numPeriodos+1] = 0;
        //Comprimento de cada objeto de cada periodo
        W.resize((unsigned int)(numPeriodos+1));
        for(p = 1; p <= numPeriodos; p++) {
            W[p].resize((unsigned int)(m[p]+1));
            for(i = 1 ; i <= m[p] ; i++){
                file >> W[p][i];
            }
        }
        //Custo por unidade de comprimento de cada objeto por periodo
        c.resize((unsigned int)(numPeriodos+1));
        for(p = 1; p <= numPeriodos; p++) {
            c[p].resize((unsigned int)(m[p]+1));
            for(i = 1 ; i <= m[p] ; i++){
                file >> c[p][i];
            }
        }
        //Numero de tipo de itens demandados por periodo
        n.resize((unsigned int )(numPeriodos+1));
        for(p = 1 ; p <= numPeriodos ; p++ ) {
            file >> n[p];
        }
        //Comprimento de cada tipo de item demandado por periodo
        w.resize((unsigned int)(numPeriodos+1));
        for(p = 1 ; p <=numPeriodos ; p++ ){
            w[p].resize((unsigned int)(n[p]+1));
            for(j = 1; j <= n[p]; j++){
                file >> w[p][j];
            }
        }
        //Demanda de cada tipo de item por periodo
        d.resize((unsigned int)(numPeriodos+1));
        for(p = 1; p <= numPeriodos; p++){
            d[p].resize((unsigned int)(n[p]+1));
            for(j = 1 ; j <= n[p] ; j++){
                file >> d[p][j];
            }
        }
    }
    catch(exception &e){
        cout << "Error: " << e.what() << endl;
    }

    //Quantidades de objetos+itens disponíveis por periodo
    barM.resize((unsigned int)(numPeriodos+2));
    for(p = 1 ; p <= numPeriodos+1 ; p++ ){
        if(p==1) barM[p] = m[p];
        else barM[p] = m[p] + barM[p-1];
    }

    //Item de menor tamanho
    for(p=1;p<=numPeriodos;p++){
        for(j = 1; j <= n[p] ; j++) {
            if(wMin > w[p][j]) wMin = w[p][j];
        }
    }
    //Objeto de maior tamanho
    for( p = 1 ; p <= numPeriodos ; p++ ){
        for(j = 1; j <= m[p] ; j++) {
            if(hatW < W[p][j]) hatW = W[p][j];
        }
    }
    //Maior quantidade de itens demandados
    for( p=1; p <= numPeriodos ; p++ ){
        if(hatN < n[p]) hatN = n[p];
    }
    //custo de cada objeto e de cada sobra por periodo
    barC.resize((unsigned int)(numPeriodos+2));
    for( p = 1; p <= numPeriodos ; p++ ){
        barC[p].resize((unsigned)(barM[p]+1));
        for( i = 1 ; i <= m[p] ; i++ ){
            barC[p][i] = c[p][i];
        }
    }
    for( p = 2; p <= numPeriodos+1 ; p++ ){
        barC[p].resize((unsigned)(barM[p]+1));
        for( i = m[p]+1 ; i <= barM[p] ; i++ ){
            l = i - m[p];
            barC[p][i] = barC[p-1][l];
        }
    }
}
void imprimeEntrada(){
    int p,i,j;
    cout << "P = "  << numPeriodos << ";" << endl;
    cout << "m = [";
    for(p = 1 ; p <= numPeriodos ; p++){
        cout << m[p];
        if(p!=numPeriodos) cout << ", ";
    }
    cout << "];" << endl;

    cout << "W = [";
    for(p = 1 ; p <= numPeriodos ; p++){
        cout << "[";
        for(i = 1 ; i <= m[p] ; i++){
            cout << W[p][i];
            if(i!=m[p]) cout << ", ";
        }
        cout << "]";
        if(p!=numPeriodos) cout << "," << endl << "     ";
    }
    cout << "];" << endl;

    cout << "c = [";
    for(p = 1 ; p <= numPeriodos ; p++){
        cout << "[";
        for(i = 1 ; i <= m[p] ; i++){
            cout << c[p][i];
            if(i!=m[p]) cout << ", ";
        }
        cout << "]";
        if(p!=numPeriodos) cout << "," << endl << "     ";
    }
    cout << "];" << endl;
    cout << "n = [";
    for(p = 1 ; p <= numPeriodos ; p++){
        cout << n[p];
        if(p!=numPeriodos) cout << ", ";
    }
    cout << "];" << endl;

    cout << "w = [";
    for(p = 1 ; p <= numPeriodos ; p++){
        cout << "[";
        for(j = 1 ; j <= n[p] ; j++){
            cout << w[p][j];
            if(j!=n[p]) cout << ", ";
        }
        cout << "]";
        if(p!=numPeriodos) cout << "," << endl << "     ";
    }
    cout << "];" << endl;

    cout << "d = [";
    for(p = 1 ; p <= numPeriodos ; p++){
        cout << "[";
        for(j = 1 ; j <= n[p] ; j++){
            cout << d[p][j];
            if(j!=n[p]) cout << ", ";
        }
        cout << "]";
        if(p!=numPeriodos) cout << "," << endl << "     ";
    }
    cout << "];" << endl;
}

void resolveModelo(){
    auto inicio = MyClock();
    IloEnv env;
    int i,j,p;
    float tam;
    char str[30];
    vector<float> maiorObjetoEntrada(numPeriodos+1,0);
    double PESO = 0.0;
    try {
        IloModel modelo(env, "Cutting");
        IloCplex cplex(env);

        /** Variaveis de decisão do modelo **/
        NumVarMatrix3D x(env, numPeriodos+1);
        NumVarMatrix2D u(env, numPeriodos+1);
        NumVarMatrix2D r(env, numPeriodos+1);
        NumVarMatrix2D alpha(env, numPeriodos+1);
        NumVarMatrix2D barW(env, numPeriodos+1);

        for(p = 1; p <= numPeriodos; p++ ){
            x[p] = NumVarMatrix2D(env, barM[p]+1);
            for( i = 1 ; i <= barM[p] ; i++ ){
                x[p][i] = IloNumVarArray(env, n[p]+1, 0,  IloInfinity, ILOINT);
            }
        }
        for(p = 1; p <= numPeriodos; p++ ){
            u[p] = IloNumVarArray(env, barM[p]+1, 0, 1, ILOBOOL);
        }

        for(p = 1; p <= numPeriodos; p++ ){
            r[p] = IloNumVarArray(env, barM[p]+1,  0.0, IloInfinity, ILOFLOAT);
        }

        for(p = 1; p <= numPeriodos; p++ ){
            alpha[p] = IloNumVarArray(env, barM[p]+1, 0, 1, ILOBOOL);
        }

        for(p = 1; p <= numPeriodos+1; p++ ){
            barW[p] = IloNumVarArray(env, barM[p]+1,  0.0, IloInfinity, ILOFLOAT);
        }

        IloExpr CustoObjetosUtilizados(env);
        IloExpr ValorSobrasRemanescentes(env);
        IloExpr funcaoObjetivo(env);
        IloExpr objetosComprados(env);
        for(p = 1; p<=numPeriodos ; p++){
            for(i = 1; i <= m[p] ; i++ ){
                if(W[p][i] > maiorObjetoEntrada[p]) maiorObjetoEntrada[p] = W[p][i];
                CustoObjetosUtilizados += c[p][i]*W[p][i]*u[p][i];
                PESO += c[p][i]*W[p][i];
                objetosComprados += u[p][i];

            }
        }
        for(i = 1; i <= barM[numPeriodos + 1] ; i++ ){
            ValorSobrasRemanescentes += barC[numPeriodos+1][i]*barW[numPeriodos+1][i];
        }
        funcaoObjetivo = (PESO*CustoObjetosUtilizados - ValorSobrasRemanescentes);
        //=====================================================Funcao Objetivo============================================//
        modelo.add(IloMinimize(env, funcaoObjetivo));
        //=====================================================Restricoes=================================================//

        IloRangeArray restricoes(env);
        //forall(p in numPeriodos, i in 1..m_bar[p], j in 1..n[p])
        //  u[p][i]*hatN >= x[p][i][j];
        for(p = 1 ; p <= numPeriodos ; p++){
            for(i = 1 ; i <= barM[p] ; i++) {
                for (j = 1; j <= n[p]; j++) {
                    restricoes.add(u[p][i]*hatN - x[p][i][j] >= 0);
                }
            }
        }

        //forall(p in numPeriodos, i in 1..m[p])
        //  barW[p][i] == W[p][i]*u[p][i];
        for( p = 1 ; p <= numPeriodos ; p++){
            for(i = 1 ; i <= m[p] ; i++){
                restricoes.add(barW[p][i]  - W[p][i]*u[p][i] == 0 );
            }
        }

        //forall(p in numPeriodos, i in 1..m_bar[p])
        //  sum(j in 1..n[p]) w[p][j]*x[p][i][j] <= barW[p][i];
        for( p = 1 ; p <= numPeriodos ; p++){
            for(i = 1 ; i <= barM[p] ; i++){
                IloExpr somatorio(env);
                for( j = 1 ; j <= n[p] ; j++){
                    somatorio += w[p][j]*x[p][i][j];
                }
                restricoes.add(somatorio - barW[p][i] <= 0);
                somatorio.end();
            }
        }

        //forall(p in numPeriodos, j in 1..n[p])
        //   sum(i in 1..m_bar[p]) x[p][i][j] == d[p][j];
        for(p = 1 ; p <=numPeriodos ; p++) {
            for (j = 1; j <= n[p]; j++) {
                IloExpr somatorio(env);
                for (i = 1; i <= barM[p]; i++) {
                    somatorio += x[p][i][j];
                }
                restricoes.add(somatorio - d[p][j] == 0);
                somatorio.end();
            }
        }

        //forall(p in numPeriodos, i in 1..m_bar[p])
        //  barW[p][i] - sum(j in 1..n[p]) w[p][j]*x[p][i][j] == r[p][i];
        for(p = 1 ; p <=numPeriodos ; p++) {
            for (i = 1; i <= barM[p]; i++) {
                IloExpr somatorio(env);
                for (j = 1; j <= n[p]; j++) {
                    somatorio += w[p][j]*x[p][i][j];
                }
                restricoes.add(barW[p][i] - somatorio - r[p][i] == 0);
                somatorio.end();
            }
        }

        //forall(p in numPeriodos, i in 1..m_bar[p]){
        //    r[p][i] <= barW[p][i];
        //    r[p][i] <= WMax*u[p][i];
        //}
        for( p = 1; p <= numPeriodos ; p++){
            for(i = 1 ; i <= barM[p] ; i++){
                restricoes.add(r[p][i] - barW[p][i] <= 0);
                restricoes.add(r[p][i] - hatW * u[p][i] <= 0);
            }
        }

        //forall(p in numPeriodos, i in 1..m_bar[p]){
        //    wmin*alpha[p][i] <= r[p][i];
        //    r[p][i] <= WMax*alpha[p][i];
        //}
        for( p = 1; p <= numPeriodos ; p++){
            for(i = 1 ; i <= barM[p] ; i++){
                restricoes.add(wMin * alpha[p][i] - r[p][i] <= 0);
                restricoes.add(r[p][i] - hatW * alpha[p][i] <= 0);
            }
        }

        //forall(p in numPeriodos, i in 1..m[p]){
        //    barW[p + 1][m[p+1] + i] <= WMax*u[p][i];
        //    r[p][i] - (1 - u[p][i])*WMax <= barW[p + 1][m[p+1] + i];
        //    barW[p + 1][m[p+1] + i] <= r[p][i] + (1 - u[p][i])*WMax;
        //}
        for( p = 1 ; p <= numPeriodos ; p++) {
            for (i = 1; i <= m[p]; i++) {
                //restricao 6
                restricoes.add(barW[p + 1][m[p + 1] + i] - hatW * u[p][i] <= 0);
                //restricões 7
                restricoes.add(r[p][i] - (1 - u[p][i])*hatW - barW[p + 1][m[p + 1] + i] <= 0);
                restricoes.add(barW[p + 1][m[p + 1] + i] - (r[p][i] + (1 - u[p][i]) * hatW) <= 0);
            }
        }

        //forall(p in 2..P, i in m[p]+1..m_bar[p]){
        //    barW[p][i] - WMax*u[p][i] <= barW[p + 1][m[p+1] + i];
        //    barW[p + 1][m[p+1] + i] <= barW[p][i] + WMax*u[p][i];
        //    r[p][i] - (1 - u[p][i])*WMax <= barW[p + 1][m[p+1] + i];
        //    barW[p + 1][m[p+1] + i] <= r[p][i] + (1 - u[p][i])*WMax;
        //}
        for( p = 2 ; p <= numPeriodos + 1  ; p++ ){
            for(i = m[p]+1 ; i <= barM[p] ; i++){
                //restricao 8
                restricoes.add( barW[p][i] - hatW*u[p][i] - barW[p+1][m[p+1]+i] <= 0 );
                restricoes.add( barW[p+1][m[p+1] + i] - (barW[p][i] + hatW*u[p][i]) <= 0 );
                //restricao 9
                restricoes.add( r[p][i] - (1-u[p][i])*hatW - barW[p+1][m[p+1]+i] <= 0 );
                restricoes.add( barW[p+1][m[p+1]+i] - (r[p][i] + (1-u[p][i])*hatW) <= 0 );
            }
        }
        modelo.add(restricoes);
        cplex.extract(modelo);

        //cplex.exportModel("modelo.lp");

        if(!imprimirLogCplex)
            cplex.setOut(env.getNullStream());

        setParametrosCplex(cplex);
        cplex.exportModel("modelo.lp");
        cplex.solve();
        cout << "[###] Solver status: " << cplex.getStatus() << endl;
        IloInt accumulatedIterations = cplex.getNiterations();
        IloInt accumulatedNodes = cplex.getNnodes();
        IloNum fObjBestBound = cplex.getBestObjValue();
        IloNum fObjValue = cplex.getObjValue();
        double cplexGap = (fObjValue - fObjBestBound)/(0.0000000001 + fObjValue);
        cout << fixed << setprecision(10);
        cerr << nomeInstancia << endl;
        cerr << fObjBestBound << "\t" << cplexGap*100.0 << "%\t";
        cerr << fObjValue << "\t" << CPLEX_GETD(CustoObjetosUtilizados) << "\t" << CPLEX_GETD(ValorSobrasRemanescentes) << endl;
        cout << "[###] MIP Iterations: "       << accumulatedIterations << endl;
        cout << "[###] B&B Nodes: "            << accumulatedNodes << endl << endl;

        cout << "----------------SOLUCAO----------------\n";
        //cerr << "\t"    << CPLEX_GETD(CustoObjetosUtilizados) << endl;
        //cerr << "\t" << CPLEX_GETD(ValorSobrasRemanescentes) << endl;
        //cerr << "\t" << cplexGap*100.0 << "%";
        //cerr << "Peso : " << PESO << endl;
        //cerr << "Objetos usados: "    << CPLEX_GET(objetosComprados) << endl;
        //cerr << "\t" << (MyClock()-inicio)/CLOCKS_PER_SEC << endl;
        //cout << "As sobras devem ser maiores que : " << wMin << endl;
        //cout << endl;
        cout << fixed << setprecision(1);
        vector<float> maiorObj(p,0);
        vector<float> maiorItem(p,0);
        vector<int> Objs(p,0);
        float valorSobras = 0;
        //cout << "\\ ----------------------------- \\" << endl;
        for(p = 1; p <= numPeriodos ; p++) {
            //cout << "Periodo : " << p << endl;
            for(i = 1 ; i <= barM[p] ; i++ ){
                if(CPLEX_GET(u[p][i]) == 1 && CPLEX_GET(barW[p][i]) > 0){
                    if( maiorObj[p] < CPLEX_GET(barW[p][i])) maiorObj[p] = CPLEX_GET(barW[p][i]);
                    Objs[p] ++;
                    //if( i <= m[p] ) cout <<  "  Objeto : " ;
                    //else cout << "  Sobra : " ;
                    //cout << i << endl;
                    //cout << "       Tamanho : " << CPLEX_GET(barW[p][i]) << endl;
                    //cout << "       Itens : " << endl;
                    for( j = 1 ; j <= n[p] ; j++ ){
                        if(w[p][j] > maiorItem[p]) maiorItem[p] = w[p][j];
                        if(CPLEX_GET(x[p][i][j]) > 0){
                            //cout << "           " << CPLEX_GET(x[p][i][j]) << "x" << w[p][j] << endl;
                            valorSobras += w[p][j]*CPLEX_GET(x[p][i][j]);
                        }
                    }
                    //cout << "       Valor da sobra no Objeto: " << CPLEX_GET(barW[p][i]) - valorSobras<< endl;
                    valorSobras = 0;
                }
            }
            //cout << "\\ ----------------------------- \\" << endl;
        }

        cout << fixed << setprecision(2);
        fstream file;
        file.open("resultados.tex",ios::out);
        file << "\\documentclass[]{article}" << endl;
        file << "\\usepackage[a4paper,top=1.0cm,bottom=2cm,left=1.5cm,right=1.5cm,marginparwidth=1.75cm]{geometry}" << endl;
        file << "\\usepackage[english,brazilian]{babel}" << endl;
        file << "\\usepackage[T1]{fontenc}" << endl;
        file << "\\usepackage[utf8]{inputenc}" << endl;
        file << "\\usepackage{graphicx}" << endl;
        file << "\\usepackage{xcolor}" << endl;
        file << "\\usepackage{color}" << endl;
        file << "\\usepackage{tikz}" << endl;

        file << "\\usepackage{amsmath}" << endl;
        file << "\\title{Teste}" << endl;
        file << "\\author{}" << endl << endl;

        file << "\\begin{document}" << endl;
        file << "\\definecolor{MyRed}{HTML}{F72C25}" << endl;
        file << "\\definecolor{MyGreen}{RGB}{54,206,65}" << endl;
        file << "\\definecolor{MyBlue}{HTML}{0B88F8}" << endl << endl;
        int TamLinhaVertical;
        for(p = 1; p <= numPeriodos ; p++) {
            TamLinhaVertical = max(n[p],m[p]);
            file << "\\begin{tikzpicture}[scale=0.35]" << endl << endl;

            //linhas horizontais
            file << "\\draw[thick,dashed] (0," << maiorObjetoEntrada[p]/14 + 6 << ") -- (" << TamLinhaVertical*4 + 7 <<"," <<  maiorObjetoEntrada[p]/14 + 6 << ");" << endl;;
            file << "\\draw[thick,dashed] (0,0) -- (" << TamLinhaVertical*4 + 7 <<",0);" << endl;;
            file << "\\draw[thick,dashed] (0," << -1*(maiorItem[p]/14 + 4) << ") -- (" << TamLinhaVertical*4 + 7 << "," << -1*(maiorItem[p]/14 + 4) << ");" << endl << endl;

            file << "\\node[rotate=90] at (-2, " << ( maiorObj[p]/14 + 6)/2 << ") {Avaliable objects};" << endl;
            file << "\\node[rotate=90] at (-2," << -1*(maiorItem[p]/20 + 4)/2 << ") {Ordered items};" << endl << endl;
            file << "\\node[rotate=0] at (" << (n[p]*4 + 7)/2<< ", " << maiorObjetoEntrada[p]/14 + 7 <<") {Instant $p =  " << p << "$} " << endl;
            file << "\\draw[thick,dashed] (0," << -1*(maiorItem[p]/20 + 4) << ") -- (0," <<  maiorObjetoEntrada[p]/14 + 6 << ")" << endl << endl;
            file << "\\vspace{3cm}" << endl;
            float posX = 2;
            float posY = 2;
            for(i = 1 ; i <= m[p] ; i++ ){
                posY = W[p][i]/12;
                file << "\\draw [thick] (" << posX << "," << 2 << ") rectangle (" << posX + 4 << "," << posY << ");" << endl << endl;
                file << "\\node[rotate=0] at (" << posX + 2  << "," << posY + 0.9 << ") {${\\cal O}_{0" << i << "}$}; " << endl;
                file << "\\node[rotate=0] at (" << posX + 2  << "," << posY/2 + 1  << ") {" << W[p][i] << "};" << endl;
                posX += 5;
            }
            posX = 2;
            posY = -1*(maiorItem[p]/12 + 2);
            for(j = 1 ; j <= n[p] ; j++){
                file << "\\draw [fill=MyBlue, thick](" << posX << "," << posY << ") rectangle (" << posX + 4 << "," << posY + w[p][j]/12 << ");" << endl;
                file << "\\node[rotate=0] at (" << posX + 2 << "," << posY+ w[p][j]/24<< ") {" << w[p][j] << "};" << endl;
                posX += 5;
            }
            file << "\\draw[thick,dashed] (" << TamLinhaVertical*4 + 7 <<"," << -1*(maiorItem[p]/20 + 4) << ") -- (" << TamLinhaVertical*4 + 7 <<"," <<  maiorObjetoEntrada[p]/14 + 6 << ")" << endl << endl;
            file << "\\end{tikzpicture}" << endl << endl;
        }
        file << "\\newpage" << endl;
        for(p = 1; p <= numPeriodos ; p++) {
            file << "\\vspace{2cm}" << endl;
            file << "\\begin{tikzpicture}[scale=0.35]" << endl << endl;

            //linhas horizontais
            file << "\\draw[thick,dashed] (0," << maiorObj[p]/5 + 6 << ") -- (" << Objs[p]*4 + 6 <<"," << maiorObj[p]/5 + 6 << ");" << endl;
            file << "\\draw[thick,dashed] (0,0) -- (" << Objs[p]*4 + 6 <<",0);" << endl;

            file << "\\node[rotate=90] at (-2, " << (maiorObj[p]/5 + 6)/2 << ") { Objects/Leftovers Used};" << endl;
            file << "\\node[rotate=0] at (" << Objs[p]*2 + 3 << ", " << maiorObj[p]/5 + 7 << ") {Instant $p =  " << p << "$} " << endl;
            file << "\\draw[thick,dashed] (0,0) -- (0," << maiorObj[p]/5 + 6 << ")" << endl << endl;
            float posX = 2;
            float posY;
            for(i = 1 ; i <= barM[p] ; i++ ){
                posY = CPLEX_GET(barW[p][i])/5 + 2;
                if(CPLEX_GET(u[p][i]) == 1 && CPLEX_GET(barW[p][i]) > 0){
                    if(i > m[p]) file << "\\draw [fill = MyRed, thick] (";
                    else file << "\\draw [thick] (";
                    file << posX << "," << 2 << ") rectangle (" << posX + 4 << "," <<  posY << ");" << endl;
                    file << "\\node[rotate=0] at (" << posX + 2 << "," << posY + 0.9 << ") {${\\cal O}_{0" << i << "}$}; " << endl;
                    float posI = 2;
                    valorSobras = 0;
                    int counter;
                    for(j = 1; j <= n[p] ; j++ ){
                        counter = 0;
                        while(counter < CPLEX_GET(x[p][i][j])){
                            file << "\\draw [fill=MyBlue, thick](" << posX << "," << posI << ") rectangle (" << posX + 4 << "," << posI + w[p][j]/5 << ");" << endl;
                            file << "\\node[rotate=0] at (" << posX + 2 << "," << posI + w[p][j]/10<< ") {" << w[p][j] << "};" << endl;
                            posI += w[p][j]/5;
                            valorSobras += w[p][j];
                            counter++;
                        }
                    }
                    if(CPLEX_GET(barW[p][i]) - valorSobras >= wMin ) file << "\\node[rotate=0] at (" << posX + 2 << "," << (posY + posI)/2 << ") {" << CPLEX_GET(barW[p][i]) - valorSobras<< "};" << endl;
                    posX += 5;
                }
            }
            file << "\\draw[thick,dashed] (" << Objs[p]*4 + 6 << ",0) -- (" << Objs[p]*4 + 6 << "," << maiorObj[p]/5 + 6 << ")" << endl << endl;
            file << "\\end{tikzpicture}" << endl << endl;
        }
        file << "\\end{document}";
        file.close();
    }
    catch (IloException& ex){
        cerr << "Error: " << ex << endl;
        env.end();
        exit(0);
    }
    catch (...){
        cerr << "Error" << endl;
        env.end();
        exit(0);
    }
    env.end();
}

void setParametrosCplex(IloCplex &cplex){
    if(setLimiteMemoriaCplex) {
        cplex.setParam(IloCplex::WorkDir, ".");
        cplex.setParam(IloCplex::WorkMem, maxMemoriaCplex);
        cplex.setParam(IloCplex::NodeFileInd, 3);
    }
    if(setLimiteThreadsCplex)
        cplex.setParam(IloCplex::Threads, numThreadsCplex);

    cplex.setParam(IloCplex::TiLim, tempoLimiteCplex);
    cplex.setParam(IloCplex::ClockType, 2); //1 CPU time. 2 Wall clock time (total physical time elapsed).
    //cplex.setParam(IloCplex::EpOpt, 1e-9);
    cplex.setParam(IloCplex::EpAGap, 0.999999); //default: 1e-06
    cplex.setParam(IloCplex::EpGap, 0.0); //default: 1e-04
    //cplex.setParam(IloCplex::NumericalEmphasis, true);
    //cplex.setParam(IloCplex::PreInd, false);
    //cplex.setParam(IloCplex::AdvInd, 2);
    //cplex.setParam(IloCplex::Symmetry, 0);
}
