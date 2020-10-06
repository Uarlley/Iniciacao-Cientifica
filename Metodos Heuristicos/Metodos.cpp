#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>
#include "Util.h"

#define CPLEX_GET(X) DOUBLE_TO_INT(cplex.getValue(X))
#define CPLEX_GETD(X) cplex.getValue(X)
#define DIR "./Instancias/"
#define INSTANCIA "instancia15.txt"

int P;
float constanteDef = 3;
float constante = 3;
using namespace std;

typedef IloArray<IloNumVarArray> NumVarMatrix2D;
typedef IloArray<NumVarMatrix2D> NumVarMatrix3D;
typedef IloArray<IloNumArray>    FloatMatrix;
typedef IloArray<IloExprArray>   ExprMatrix;
typedef unsigned long long ullong;


void leEntrada(const string &arquivoEntrada);
void leArgumentos(int argc, char** argv);
void setParametrosCplex(IloCplex &cplex);
void ModeloGuloso(int p);
void resolveMetodoFF();
void ModeloFF();
void resolveMetodoBF();
void ModeloBF();
void resolveMetodoGuloso();
void imprimeEntrada();

struct obj{
    int periodo;
    int size;
    int initialSize;
    int isLeftover = 0;
    int cost;
    int wasUsed = 0;
    vector<int> itens;
    bool operator< (const obj& o) const{
        return size*pow((constante - (2*periodo)/P),cost-(P-periodo))*(1-isLeftover) <= o.size*pow((constante - (2*o.periodo)/P),o.cost-(P-o.periodo))*(1-o.isLeftover);
    }
};

struct item{
    int size;
    int demand;

    bool operator< (const item& i) const{
        return size*demand <= i.size*i.demand;
    }
};
/*********************************************************/
//Variáveis (dados) de entrada
vector<int> m; //Numero de objetos disponíveis por periodo
vector<vector<obj>> W; //Comprimento de cada objeto de cada periodo
vector<int> n; //Numero de tipo de itens demandados em cada periodo
vector<vector<item>> w; //Comprimento de cada tipo de item demadado por periodo

long long int CUSTOFINAL = 0;
long long int PESOFINAL = 0;
long long int VALORSOBRAS = 0;
int Metodo = 1;
/*********************************************************/

//Variáveis auxiliares
vector<vector<vector<int>>> x;
vector<vector<bool>> u;

string nomeInstancia;
string nomeArquivoInstancia;
int tempoLimiteCplex;
bool imprimirLogCplex;
bool setLimiteThreadsCplex;
int numThreadsCplex;
bool setLimiteMemoriaCplex;
int maxMemoriaCplex;
int Wmin = INT_MAX; //Objeto de menor tamanho

int main(int argc, char** argv) {
    auto tempoInicio = MyClock();

    leArgumentos(argc, argv);

    leEntrada(nomeInstancia);
    if(Metodo == 1) resolveMetodoGuloso();
    else if(Metodo == 2) resolveMetodoFF();
    else if(Metodo == 3) resolveMetodoBF();
    cout << "Tempo Total Gasto: " << setprecision(5) << fixed << (MyClock()-tempoInicio)/CLOCKS_PER_SEC << endl << endl;

    return 0;
}

void leArgumentos(int argc, char** argv) {
    tempoLimiteCplex = 3600*4; //4 horas
    imprimirLogCplex = false;
    setLimiteMemoriaCplex = false;
    setLimiteThreadsCplex = false;
    nomeInstancia = string(DIR) + string(INSTANCIA);
    if(argc == 1) {
        tempoLimiteCplex = 3600*4;
        imprimirLogCplex = false;
        setLimiteMemoriaCplex = true;
        maxMemoriaCplex =3000;
        setLimiteThreadsCplex = true;
        numThreadsCplex = 1;
        constanteDef = 3;
        Metodo = 2;
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
            imprimirLogCplex = false;
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
        if(string(argv[i]) == "-modelo"){
            Metodo = atoi(argv[i+1]);
            continue;
        }
        if(string(argv[i]) == "-const"){
            constanteDef = atoi(argv[i+1]);
            continue;
        }
    }
    constante = 0.1*constanteDef + 3;

    //separando o nome da instancia
    string::size_type loc  = nomeInstancia.find_last_of( "/", nomeInstancia.size() );
    string::size_type loc2 = nomeInstancia.find_last_of( ".", nomeInstancia.size() );
    if( !((loc != string::npos ) && (loc2 != string::npos)) ){
        cout << "ERRO!" << endl;
        exit(0);
    }
    nomeArquivoInstancia = string("");
    nomeArquivoInstancia.append(nomeInstancia, loc+1, loc2-loc-1);

    cout << "\n\nInstancia: " << nomeInstancia << endl;
    cout << "Tempo Limite Cplex: " << tempoLimiteCplex << endl;
    //cout << "Imprimir Log Cplex? " << std::boolalpha << imprimirLogCplex << endl;
}

void leEntrada(const string &arquivoEntrada){
    int i,j,l,p;
    try{
        ifstream file(arquivoEntrada.c_str());
        if(!file){
            cerr << "Erro ao abrir o arquivo " << arquivoEntrada << endl;
            exit(0);
        }
        //Numero de periodos
        file >> P;

        //Numero de objetos disponíveis por periodo
        m.resize(P+2);
        for(p = 1; p <= P; p++) {
            file >> m[p];
        }
        m[P+1] = 0;
        //Comprimento de cada objeto de cada periodo
        W.resize(P+1);
        for(p = 1; p <= P; p++) {
            W[p].resize(m[p]+1);
            for(i = 1 ; i <= m[p] ; i++){
                file >> W[p][i].size;
                W[p][i].isLeftover = 0;
                W[p][i].initialSize = W[p][i].size;
                W[p][i].periodo = p;
            }
        }
        //Custo por unidade de comprimento de cada objeto por periodo
        for(p = 1; p <= P; p++) {
            for(i = 1 ; i <= m[p] ; i++){
                file >> W[p][i].cost;
            }
        }
        //Numero de tipo de itens demandados por periodo
        n.resize(P+1);
        for(p = 1 ; p <= P ; p++ ) {
            file >> n[p];
        }
        //Comprimento de cada tipo de item demandado por periodo
        w.resize(P+1);
        for(p = 1 ; p <=P ; p++ ){
            w[p].resize(n[p]+1);
            for(j = 1; j <= n[p]; j++){
                file >> w[p][j].size;
            }
        }
        //Demanda de cada tipo de item por periodo
        for(p = 1; p <= P; p++){
            for(j = 1 ; j <= n[p] ; j++){
                file >> w[p][j].demand;
            }
        }

    }
    catch(exception &e){
        cout << "Error: " << e.what() << endl;
    }

    for( p = 1 ; p <= P ; p++){
        for(i = 1; i <= m[p] ;i++){
            PESOFINAL += W[p][i].cost*W[p][i].size;
        }
    }

    for( p = 1 ; p <= P ; p++){
        for(j = 1; j <= n[p] ;j++){
            if(Wmin > w[p][j].size) Wmin = w[p][j].size;
        }
    }

}
void imprimeEntrada(){
    int p,i,j;
    cout << "P = "  << P << ";" << endl;
    cout << "m = [";
    for(p = 1 ; p <= P ; p++){
        cout << m[p];
        if(p!=P) cout << ", ";
    }
    cout << "];" << endl;

    cout << "W = [";
    for(p = 1 ; p <= P ; p++){
        cout << "[";
        for(i = 1 ; i <= m[p] ; i++){
            cout << W[p][i].size;
            if(i!=m[p]) cout << ", ";
        }
        cout << "]";
        if(p!=P) cout << "," << endl << "     ";
    }
    cout << "];" << endl;

    cout << "custo = [";
    for(p = 1 ; p <= P ; p++){
        cout << "[";
        for(i = 1 ; i <= m[p] ; i++){
            cout << W[p][i].cost;
            if(i!=m[p]) cout << ", ";
        }
        cout << "]";
        if(p!=P) cout << "," << endl << "     ";
    }
    cout << "];" << endl;
    cout << "n = [";
    for(p = 1 ; p <= P ; p++){
        cout << n[p];
        if(p!=P) cout << ", ";
    }
    cout << "];" << endl;

    cout << "w = [";
    for(p = 1 ; p <= P ; p++){
        cout << "[";
        for(j = 1 ; j <= n[p] ; j++){
            cout << w[p][j].size;
            if(j!=n[p]) cout << ", ";
        }
        cout << "]";
        if(p!=P) cout << "," << endl << "     ";
    }
    cout << "];" << endl;

    cout << "d = [";
    for(p = 1 ; p <= P ; p++){
        cout << "[";
        for(j = 1 ; j <= n[p] ; j++){
            cout << w[p][j].demand;
            if(j!=n[p]) cout << ", ";
        }
        cout << "]";
        if(p!=P) cout << "," << endl << "     ";
    }
    cout << "];" << endl;
}


void ModeloGuloso(int periodo) {
    auto inicio = MyClock();
    IloEnv env;
    int i, j,p;
    char str[30];

    double PESO = 0.0;

    int objetos; //Numero de objetos disponíveis
    vector <obj> tamObjetos; //Comprimento de cada objeto
    int itens; //Numero de tipo de itens demandados
    vector<item> tamItens; //Comprimento de cada tipo de item demadado

    objetos = m[periodo];
    itens = n[periodo];
    tamObjetos.resize(objetos + 1);
    for (i = 1; i <= objetos; i++) {
        tamObjetos[i].size = W[periodo][i].size;
        tamObjetos[i].cost = W[periodo][i].cost;
        tamObjetos[i].isLeftover = W[periodo][i].isLeftover;
    }
    tamItens.resize(itens + 1);
    for (j = 1; j <= itens; j++) {
        tamItens[j].size = w[periodo][j].size;
        tamItens[j].demand = w[periodo][j].demand;
    }
    for (i = 1; i <= objetos; i++)
        PESO += tamObjetos[i].cost * tamObjetos[i].size * (1 - tamObjetos[i].isLeftover);

    try {
        IloModel modelo(env, "Cutting");
        IloCplex cplex(env);

        /** Variaveis de decisão do modelo **/
        NumVarMatrix2D x(env, objetos + 1);
        IloNumVarArray u(env, objetos + 1, 0, 1, ILOBOOL);
        IloNumVarArray r(env, objetos + 1, 0.0, IloInfinity, ILOINT);
        IloNumVarArray alpha(env, objetos + 1, 0, 1, ILOBOOL);

        for (i = 1; i <= objetos; ++i)
            x[i] = IloNumVarArray(env, itens + 1, 0, IloInfinity, ILOINT);
        /***********************************/

        IloExpr CustoObjetosUtilizados(env);
        IloExpr ValorSobras(env);
        IloExpr funcaoObjetivo(env);
        IloExpr objetosUsados(env);
        for (i = 1; i <= objetos; ++i) {
            CustoObjetosUtilizados += tamObjetos[i].cost * tamObjetos[i].size * u[i] * (1 - tamObjetos[i].isLeftover);
            ValorSobras += tamObjetos[i].cost * r[i];
            objetosUsados += u[i];
        }

        funcaoObjetivo = (PESO * CustoObjetosUtilizados - ValorSobras);

        //=====================================================Funcao Objetivo
        modelo.add(IloMinimize(env, funcaoObjetivo));

        //=====================================================Restricoes
        IloRangeArray restricoes(env);


        /*
        forall(i in objetos){
            sum(j in itens) tamItens[j].size*x[i][j] <= tamObjetos.size[i]*u[i];
        }
        */
        for (i = 1; i <= objetos; ++i) {
            IloExpr somatorio(env);
            for (j = 1; j <= itens; ++j)
                somatorio += tamItens[j].size * x[i][j];
            restricoes.add(somatorio - tamObjetos[i].size * u[i] <= 0);
            somatorio.end();
        }

        /*
        forall(j in itens){
            sum(i in objetos) x[i][j] == tamItens[j].demand;
        }
        */
        for (j = 1; j <= itens; ++j) {
            IloExpr somatorio(env);
            for (i = 1; i <= objetos; ++i)
                somatorio += x[i][j];
            restricoes.add(somatorio - tamItens[j].demand == 0);
            somatorio.end();
        }

        /*
        forall(i in objetos){
            tamObjetos[i].size*u[i] - sum(j in itens) tamItens[j].size*x[i][j] >= r[i];
            r[i] >= Wmin*u[i];
        nizei
        */
        for (i = 1; i <= objetos; ++i) {
            IloExpr somatorio(env);
            for (j = 1; j <= itens; ++j)
                somatorio += tamItens[j].size * x[i][j];
            restricoes.add(tamObjetos[i].size * u[i] - somatorio - r[i] == 0);
            somatorio.end();
        }

        modelo.add(restricoes);
        cplex.extract(modelo);

        //cplex.exportModel("modelo.lp");

        if (!imprimirLogCplex)
            cplex.setOut(env.getNullStream());

        setParametrosCplex(cplex);

        cplex.solve();
        IloInt accumulatedIterations = cplex.getNiterations();
        IloInt accumulatedNodes = cplex.getNnodes();
        IloNum fObjBestBound = cplex.getBestObjValue();
        IloNum fObjValue = cplex.getObjValue();
        double cplexGap = (fObjValue - fObjBestBound) / (0.0000000001 + fObjValue);
        cout << fixed << setprecision(10);
        cout << "Custo dos objetos: " << CPLEX_GETD(CustoObjetosUtilizados) << endl;
        cout << "Valor das sobras: " << CPLEX_GETD(ValorSobras) << endl;
        cout << "Objetos usados: " << CPLEX_GET(objetosUsados) << endl;
        CUSTOFINAL += CPLEX_GETD(CustoObjetosUtilizados);
        for (i = 1; i <= objetos; i++) {
            obj aux;
            if ((CPLEX_GET(r[i]) > Wmin) && periodo < P) {
                aux.size = CPLEX_GET(r[i]);
                aux.cost = tamObjetos[i].cost;
                aux.isLeftover = 1;
                aux.periodo = p;
                W[periodo + 1].push_back(aux);
                m[periodo + 1]++;
            }
        }
        if (periodo == P) {
            cerr << "Função Objetivo: " << PESOFINAL * CUSTOFINAL - CPLEX_GETD(ValorSobras) << endl;
        }
        cout << "Periodo : " << periodo << endl;
        for (i = 1; i <= m[periodo]; i++) {
            if (CPLEX_GET(u[i]) == 1) {
                if (tamObjetos[i].isLeftover == 1) cout << "    Sobra: ";
                else cout << "    Objeto: ";
                cout << tamObjetos[i].size << endl;
                cout << "    Itens: " << endl;
                for (j = 1; j <= n[periodo]; j++) {
                    if (CPLEX_GET(x[i][j]) > 0) {
                        cout << "        " << tamItens[j].size << "x" << CPLEX_GET(x[i][j]) << endl;
                    }
                }
                cout << "    Sobra no objeto: " << CPLEX_GET(r[i]) << endl;
            }
        }
        if(periodo==P){
            for( i =1;i <= m[periodo];i++){
                if(CPLEX_GET(r[i]) >= Wmin) VALORSOBRAS += CPLEX_GET(r[i])*tamObjetos[i].cost;
            }
            cout << "Valor total das sobras: " << VALORSOBRAS << endl;
            cout << "Objetivo obtido: " << PESOFINAL*CUSTOFINAL - VALORSOBRAS << endl;
        }
    }
    catch (IloException & ex){
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

void resolveMetodoGuloso(){
    cout << "Sobras devem ser maiores que " << Wmin << endl;
    cout << "Modelo = 1" << endl;
    for(int i = 1 ; i <= P ; i++){
        cout << "//------------------------------------ PERIODO " << i << " ------------------------------------//" << endl;
        ModeloGuloso(i);
    }
    cout << "PESO : " << PESOFINAL << endl;
    cout << "Custo Objetos Utilizados :" << CUSTOFINAL << endl;

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
void MetodoFF(){
    int i,j,k,p;
    obj aux;
    for(p=1;p<=P;++p){
        sort(W[p].begin()+1,W[p].end());
        k = 1;
        for(i=1;i<=m[p];i++){
            (W[p][i].itens).resize(n[p]+1);
            for(j=1 ; j<=n[p] ; j++) W[p][i].itens[j] = 0;
        }
        for(i=1;i<=n[p];i++){
            for(j=1;j<=w[p][i].demand;j++){
                while(w[p][i].size > W[p][k].size){
                    k++;
                    if(k > m[p]){
                        exit(1);
                    }
                }
                if(W[p][k].wasUsed == 0){
                    W[p][k].wasUsed = 1;
                    CUSTOFINAL += W[p][k].size*W[p][k].cost*(1 - W[p][k].isLeftover);
                }
                W[p][k].size -= w[p][i].size;
                W[p][k].itens[i] += 1;
                k = 1;
            }
        }
        if(p<P){
            for(k=1;k<=W[p].size();k++){
                cout << k << endl;
                if(W[p][k].size >= Wmin && (W[p][k].isLeftover == 1 || W[p][k].wasUsed==1)){
                    aux.size = W[p][k].size;
                    aux.isLeftover = 1;
                    aux.cost = W[p][k].cost;
                    aux.initialSize = aux.size;
                    aux.periodo = p+1;
                    W[p+1].push_back(aux);
                    m[p+1]++;
                }
            }
        }
    }
}

void resolveMetodoFF(){
    int i,j,p;
    long long int ValorSobras = 0;
    cout << "Modelo = 2" << endl;
    MetodoFF();
    cout << "PESO :" << PESOFINAL << endl;
    cout << "Custo dos Objetos Utilizados: " << CUSTOFINAL << endl;
    for(i=1;i<=m[P];i++){
        if(W[P][i].isLeftover == 1 || (W[P][i].wasUsed == 1 && W[P][i].size > 0)){
            ValorSobras+= W[P][i].cost*W[P][i].size;
        }
    }
    cout << "Valor das sobras restantes: " << ValorSobras << endl;
    cout << "Objetivo obtido: "  << PESOFINAL*CUSTOFINAL - ValorSobras << endl;
    cerr << CUSTOFINAL << " "  << PESOFINAL*CUSTOFINAL - ValorSobras << " " << ValorSobras << endl;
    cout << constante << endl;
    for(p = 1 ; p <= P; p++){
        CUSTOFINAL = 0;
        cout << "Periodo : " << p << endl;
        for(i = 1 ; i<=m[p] ; i++){
            ValorSobras = 0;
            if(W[p][i].wasUsed == 1){
                if(W[p][i].isLeftover == 1) cout << "    Sobra: " ;
                else cout << "    Objeto: ";
                cout << W[p][i].initialSize << endl;
                CUSTOFINAL += W[p][i].initialSize*W[p][i].cost*(1 - W[p][i].isLeftover);
                cout << "    Itens: " << endl;
                for(j = 1 ; j <= n[p] ; j++){
                    if(W[p][i].itens[j] > 0){
                        cout << "        " << w[p][j].size << "x" << W[p][i].itens[j] << endl;
                        ValorSobras += w[p][j].size*W[p][i].itens[j];
                    }
                }
                cout << "    Sobra: " << W[p][i].initialSize - ValorSobras << endl;
            }
        }
        cout << "Custo dos Objetos: " << CUSTOFINAL << endl;
        cout << "//--------------------------------------------//" << endl;
    }
}

void ModeloBF(){
    int i,j,k,l,p;
    obj aux;
    for( p = 1; p <= P;p++){
        for(i = 1; i <= m[p] ; i++){
            (W[p][i].itens).resize(n[p]+1);
            for(j = 1 ; j <= n[p] ; j++) W[p][i].itens[j] = 0;
        }
        k = 1;
        for(i=1;i<=n[p];i++){
            for(j=1;j<=w[p][i].demand;j++){
                sort(W[p].begin()+1,W[p].end());
                while(w[p][i].size > W[p][k].size) k++;
                if(W[p][k].wasUsed == 0){
                    W[p][k].wasUsed = 1;
                    CUSTOFINAL += W[p][k].size*W[p][k].cost*(1 - W[p][k].isLeftover);
                }
                W[p][k].size -= w[p][i].size;
                W[p][k].itens[i] ++;
                k = 1;
            }
        }
        if(p<P){
            for(k=1;k<=W[p].size();k++){
                if(W[p][k].size >= Wmin && (W[p][k].isLeftover == 1 || W[p][k].wasUsed ==1)){
                    aux.size = W[p][k].size;
                    aux.isLeftover = 1;
                    aux.cost = W[p][k].cost;
                    aux.initialSize = aux.size;
                    aux.periodo = p+1;
                    W[p+1].push_back(aux);
                    m[p+1]++;
                }
            }
        }
    }
}

void resolveMetodoBF(){
    int i,j,p;
    cout << "Modelo = 3" << endl;
    ModeloBF();
    int ValorSobras = 0;
    cout << "Sobras devem ser maiores que " << Wmin << endl;
    cout << "PESO :" << PESOFINAL << endl;
    cout << "Custo dos Objetos Utilizados: " << CUSTOFINAL << endl;
    for(i=1;i<=m[P];i++){
        if(W[P][i].isLeftover == 1 || (W[P][i].wasUsed == 1 && W[P][i].size > 0)) ValorSobras+= W[P][i].cost*W[P][i].size;
    }
    cout << "Valor das sobras restantes: " << ValorSobras << endl;
    cout << "Objetivo obtido: "  << PESOFINAL*CUSTOFINAL - ValorSobras << endl;
    for(p = 1 ; p <= P; p++){
        //cout << "Periodo : " << p << endl;
        for(i = 1 ; i <= m[p] ; i++){
            ValorSobras = 0;
            if(W[p][i].wasUsed == 1){
                //if(W[p][i].isLeftover == 1) cout << "    Sobra: " ;
                //else cout << "    Objeto: ";
                //cout << W[p][i].initialSize << endl;
                //cout << "    Itens: " << endl;
                for(j = 1 ; j <= n[p] ; j++){
                    if(W[p][i].itens[j] > 0) {
                        //cout << "        " << w[p][j].size << "x" << W[p][i].itens[j] << endl;
                        ValorSobras += w[p][j].size*W[p][i].itens[j];
                    }
                }
                //cout << "    Sobra no Objeto: " << W[p][i].initialSize - ValorSobras << endl;
            }
        }
        //cout << "//--------------------------------------------//" << endl;
    }
}
