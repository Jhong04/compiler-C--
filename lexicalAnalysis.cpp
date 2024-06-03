#include<iostream>
#include<string>
#include<vector>
#include<stack>

using namespace std;

int director=0;

//--------------------------------------------------------------------------------------------
//词法分析器
//注：源代码每个单词符号间必须用空格分隔

FILE* fp;				//文件指针

enum Type{
	TP_KW,
	TP_NUM,
	TP_OP,
	START
};

enum TokenCode
{
	/*未定义*/
	TK_UNDEF = 0,

	/* 关键字 */
	KW_WHILE,	
    KW_IF,
    KW_ELSE,
    KW_INT,

	/* 运算符 */
	TK_PLUS,	//+加号
	TK_MINUS,	//-减号
	TK_STAR,	//*乘号
	TK_DIVIDE,	///除号
    TK_MOD,     //取余
	TK_ASSIGN,	//=赋值运算符
	TK_EQ,		//==等于号
    TK_NOTEQ,   //!=不等于
	TK_LT,		//<小于号
	TK_LEQ,		//<=小于等于号
	TK_GT,		//>大于号
	TK_GEQ,		//>=大于等于号
	TK_AND,		//&&
    TK_OR,		//||
    TK_NOT,		//!
    TK_DECR,    //--自减
    TK_INCR,    //++自增

	/* 分隔符 */
	TK_OPENPA,	//(左圆括号
	TK_CLOSEPA,	//)右圆括号
	TK_OPENBR,	//[左中括号
	TK_CLOSEBR,	//]右中括号
	TK_BEGIN,	//{左大括号
	TK_END,		//}右大括号
	TK_COMMA,	//,逗号
	TK_SEMOCOLOM,	//;分号
	
	/* 常量 */
	TK_INT,		//整型常量

	/* 标识符 */
	TK_IDENT,

	/*开始符*/
	TK_START,
	//结束符
	TK_EOF
};

struct Word{
	int code;
	string value;
	string attribute;
};


TokenCode code = TK_UNDEF;		//记录单词的种别码
const int keyMAX = 4;	//关键字数量
const int opMAX = 25;
int row = 1;	//所在行数

string keyWord[keyMAX]={"while","if","else","int"};//记录关键字
string op[opMAX]={"+","-","*","/","%","=","==","!=",
"<","<=",">",">=","&&","||","!","--","++","(",")",
"[","]","{","}",",",";"};
//判断是否为关键字
bool isKey(Word& w)
{
	string token=w.value;
	for (int i = 0; i < keyMAX; i++)
	{
		if (token.compare(keyWord[i]) == 0)
		{
			w.code=i+1;
			return true;
		}
			
	}
	w.code=TK_IDENT;
	return false;
}

//判断字符是否为字母
bool isLetter(char letter)
{
	if ((letter >= 'a'&&letter <= 'z') || (letter >= 'A' &&letter <= 'Z'))
		return true;
	return false;

}

//判断字符是否为数字
bool isDigit(char digit)
{
	if (digit >= '0'&&digit <= '9')
		return true;
	return false;
}

//标识当前处理的单词，用于调试
string curW;

Word lexicalAnalysis(string token,Type tp){
	Word w;
	w.value=token;
	curW=token;
	switch(tp){
		case TP_KW:{
			isKey(w);
			break;
		}
		case TP_NUM:{
			w.code=TK_INT;
			break;
		}
		case TP_OP:{
			for(int i=0;i<opMAX;i++){
				if(token.compare(op[i])==0){
					w.code=i+5;
					break;
				}
			}
			break;
		}
		case START:{
			w.code=TK_START;
			break;
		}

	}

	//cout<<w.value<<" "<<w.code<<" "<<w.attribute<<endl;
	return w;
	
}


bool isEOF=false;
void rollBack(Word& w) { // 回退一个单词 + ' '
    director--;
	if(!isEOF){
		string value = w.value;
		int len = value.length();

		// 首先回退一个空格
		ungetc(' ', fp);

		// 从后向前回退每个字符
		for (int i = len - 1; i >= 0; i--) {
			ungetc(value[i], fp);
		}
	}else{
		ungetc(EOF,fp);
	}
}


Word getNextWord() {
    director++;
    Type tp;
    char ch;
    string token;


    while ((ch = static_cast<char>(fgetc(fp))) != EOF) {
        // 忽略空格、Tab 和回车
        if (ch == ' ' || ch == '\t' || ch == '\n') {
            if (ch == '\n') {
                row++;  // 遇到换行符，记录行数的 row 加 1
            }
            continue;  // 继续执行循环
        }
        
        // 处理关键字或标识符
        if (isLetter(ch)) {
            token.clear();  // 初始化 token
            do {
                token.push_back(ch);  // 将读取的字符 ch 存入 token 中
                ch = fgetc(fp);       // 获取下一个字符
            } while (isLetter(ch) || isDigit(ch));

            ungetc(ch, fp);  // 回退最后一个读取的非字母数字字符
            tp = TP_KW;
            return lexicalAnalysis(token, tp);
        }
        
        // 处理常数
        if (isDigit(ch)) {
            token.clear();  // 初始化 token
            do {
                token.push_back(ch);  // 将读取的字符 ch 存入 token 中
                ch = fgetc(fp);       // 获取下一个字符
            } while (isDigit(ch));

            ungetc(ch, fp);  // 回退最后一个读取的非数字字符
            tp = TP_NUM;
            return lexicalAnalysis(token, tp);
        }
        
        // 处理分隔符或运算符
        token.clear();
        token.push_back(ch);
		if(ch==EOF){
			isEOF=true;
			break;
		}else
        	ch = static_cast<char>(fgetc(fp));
		
        // 处理双字符运算符
        if (!isspace(ch) && ch != EOF) {
            // 暂存字符，以防它包含组合运算符
            token.push_back(ch);

            // 检查是否为双字符运算符
            if (token == "==" || token == "!=" || token == "<=" || token == ">=" || token == "&&" || token == "||") {
                // 确认双字符运算符，不需要回退字符
            } else {
                // 不是双字符运算符，回退最后一个字符
                token.pop_back();
                ungetc(ch, fp);
            }
        } else if(ch==' ') {
            // 如果第二个字符是空白字符或 EOF，直接回退
            ungetc(ch, fp);
        }

        // 更新换行行号
        if (ch == '\n') {
            row++;
        }
        tp = TP_OP;
        return lexicalAnalysis(token, tp);
    }
	
	//当前ch==EOF
	isEOF=true;
	Word w;
	w.value=';';
	w.code=TK_SEMOCOLOM;
	return w;  
}

//-----------------------------------------------------------------------------------------
//语义分析器

//属性组，用于传递单词符号属性（非终结符|终结符）
struct AttrGroup{
	string place;
	string inArray;
};

//四元式
struct Quad {
	string op;
	string str1;
	string str2;
	string result;
};

vector<Quad>quads;

//标记临时变量标号
int tempVarNum=1;

//生成四元式
void emit(string op,string str1,string str2,string result){
	quads.push_back(Quad{op,str1,str2,result});
}

//生成一个临时变量名
string newtemp(){
	string x="t"+to_string(tempVarNum);
	tempVarNum++;
	return x;
}

//输出语义分析结果
void SemanticAnalyzer(){
	int i=0;
	for(auto q:quads){
		cout<<i<<" : "<<q.op<<" "<<q.str1<<" "<<q.str2<<" "<<q.result<<endl;
		i++;
	}
}

//-----------------------------------------------------------------------------------------
//语法分析器
AttrGroup expr();
AttrGroup term();
AttrGroup rest5(string inArray);
AttrGroup rest6(string inArray);
AttrGroup factor();
AttrGroup unary();

void equality();
void rest4();
void rel();
void rop_expr();
void stmts();
void rest0();
void stmt();
AttrGroup loc();
void boolExpr();
AttrGroup resta(string inArray);
void elist();
void rest1();


void error(){
	cout<<"error"<<endl;
	exit(0);
}

void success(){
	cout<<"success"<<endl;
}


/*产生式：
stmts -> stmt rest0
rest0 -> stmt rest0|$
stmt -> loc = expr ;
loc -> id resta 
resta -> [elist] | $

elist -> expr rest1
resft1 -> , expr rest1|$
bool -> equality
equality -> rel rest4
rest4 ->  == rel rest4|!= rel rest4|$
rel -> expr rop_expr
rop_expr -> < expr|<=expr|>expr|>=expr

expr -> term rest5 ;
rest5 -> + term rest5|- term rest5|$
term -> unary rest6
rest6 -> * unary rest6|/ unary rest6|$
unary -> factor
factor-> (expr)|loc|num
*/

AttrGroup expr(){
	AttrGroup attr;
	cout<<"expr -> term rest5"<<endl;
	string term_place = term().place;
	string rest5_place=rest5(term_place).place;
	attr.place=rest5_place;
	return attr;
	
}

AttrGroup term(){
	AttrGroup attr;
	cout<<"term->unary rest6"<<endl;
	string unary_place= unary().place;
	string rest6_place=rest6(unary_place).place;
	attr.place=rest6_place;
	return attr;
}

AttrGroup rest5(string inArray){
	AttrGroup attr;
	Word w=getNextWord();
	if(w.code==TK_PLUS){//+
		cout<<"rest5 -> + term rest5"<<endl;
		string rest5_1_inArray=newtemp();
		string term_place=term().place;
		emit("+",inArray,term_place,rest5_1_inArray);
		string rest5_1_place=rest5(rest5_1_inArray).place;
		attr.place=rest5_1_place;
		return attr;

	}else if(w.code==TK_MINUS){//-
		cout<<"rest5 -> - term rest5"<<endl;
		string rest5_1_inArray=newtemp();
		string term_place=term().place;
		emit("-",inArray,term_place,rest5_1_inArray);
		string rest5_1_place=rest5(rest5_1_inArray).place;
		attr.place=rest5_1_place;
		return attr;
	}else {
		rollBack(w);
		cout<<"rest5 -> $"<<endl;	
		string rest5_place=inArray;
		attr.place=rest5_place;
		return attr;
	}
}

AttrGroup rest6(string inArray){
	Word w=getNextWord();
	AttrGroup attr;
	if(w.code==TK_STAR){//*
		cout<<"rest6 -> * unary rest6"<<endl;
		string rest6_1_inArray=newtemp();
		string unary_place=unary().place;
		emit("*",inArray,unary_place,rest6_1_inArray);
		string rest6_1_place=rest6(rest6_1_inArray).place;
		attr.place=rest6_1_place;
		return attr;
	}else if(w.code==TK_DIVIDE){///
		cout<<"rest6 -> / unary rest6"<<endl;
		string rest6_1_inArray=newtemp();
		string unary_place=unary().place;
		emit("/",inArray,unary_place,rest6_1_inArray);
		string rest6_1_place=rest6(rest6_1_inArray).place;
		attr.place=rest6_1_place;
		return attr;
	}else {
		rollBack(w);
		cout<<"rest6 -> $"<<endl;
		string rest6_place=inArray;
		attr.place=rest6_place;
		return attr;
	}
}

AttrGroup unary(){
	AttrGroup attr;
	cout<<"unary->factor"<<endl;
	string unary_place=factor().place;
	attr.place=unary_place;
	return attr;
}

AttrGroup factor(){
	Word w=getNextWord();
	AttrGroup attr;
	if(w.code==TK_OPENPA){
		cout<<"factor -> (expr)"<<endl;
		string factor_place=expr().place;
		w=getNextWord();
		if(w.code!=TK_CLOSEPA){
			error();
			rollBack(w);
		}
		attr.place=factor_place;
		return attr;
	}else if(w.code==30){
		cout<<"factor->num"<<endl;
		string factor_place=w.value;
		attr.place=factor_place;
		return attr;
	}
	else{
		rollBack(w);
		cout<<"factor -> loc"<<endl;
		string loc_place=loc().place;
		string factor_place=loc_place;
		attr.place=factor_place;
		return attr;
	}

}

//匹配布尔表达式

void boolExpr(){
	cout<<"bool->equality"<<endl;
	equality();
}

void equality(){
	cout<<"equality->rel rest4"<<endl;
	rel();
	rest4();
}

void rest4(){
	Word w=getNextWord();
	if(w.code==TK_EQ){
		cout<<"rest4-> == rel rest4"<<endl;
		rel();
		rest4();
	}
	else if(w.code==TK_NOTEQ){
		cout<<"rest4 -> != rel rest4"<<endl;
		rel();
		rest4();
	}else
	{
		rollBack(w);
		cout<<"rest4 -> $"<<endl;
	}
}

void rel(){
	cout<<"rel->expr rop_expr"<<endl;
	expr();
	rop_expr();
}

void rop_expr(){
	Word w=getNextWord();
	int code=w.code;
	if(code==TK_LT){
		cout<<"rop_expr -> < expr"<<endl;
		expr();
	}else if(code==TK_LEQ){
		cout<<"rop_expr -> <= expr"<<endl;
		expr();
	}else if(code==TK_GT){
		cout<<"rop_expr -> > expr"<<endl;
		expr();
	}else if(code==TK_GEQ){
		cout<<"rop_expr -> >= expr"<<endl;
		expr();
	}else{
		rollBack(w);
		cout<<"rop_expr -> $"<<endl;
	}
}

void stmts(){
	cout<<"stmts -> stmt rest0"<<endl;
	stmt();
	rest0();
}

void stmt(){
	Word w=getNextWord();

	if (w.code == KW_IF) {
			cout<<"stmt -> if(bool) stmt else stmt"<<endl;
			w=getNextWord();
			if(w.code==TK_OPENPA)
				boolExpr();
			else
				error();
			w=getNextWord();
			if(w.code==TK_CLOSEPA)
				 stmt();
			else 
				error();
            w = getNextWord();
            if (w.code == KW_ELSE) {
                stmt();
            }
        } else if (w.code == KW_WHILE) {
			cout<<"stmt -> while(bool) stmt"<<endl;
			w=getNextWord();
			if(w.code==TK_OPENPA){
				boolExpr();
			}else
				error();
			w=getNextWord();
			if(w.code==TK_CLOSEPA){
				stmt();
			}else
				error();
    }else{
		rollBack(w);
		cout<<"stmt -> loc = expr ;"<<endl;
		string loc_place=loc().place;
		w=getNextWord();
		if(w.code!=TK_ASSIGN)
			error();
		string expr_place=expr().place;
		w=getNextWord();
		if(w.code!=TK_SEMOCOLOM)
			error();
		emit("=",expr_place," ",loc_place);
	}
    
}

AttrGroup loc(){
	Word w=getNextWord();
	AttrGroup attr;
	if(w.code==TK_IDENT){
		cout<<"loc -> id resta"<<endl;
		string resta_plcae=resta(w.value).place;
		attr.place=resta_plcae;
	}else{
		error();
	}
	return attr;
}

void rest0(){
	    Word w=getNextWord();
		rollBack(w);//先读取下一字符在回退，来探测下一字符是否为EOF，若为EOF则不再继续递归语法树
		if(!isEOF){
			cout<<"rest0 -> stmt rest0"<<endl;
			stmt();
			rest0();
		}

}

AttrGroup resta(string inArray){
	AttrGroup attr;
	Word w=getNextWord();
	if(w.code==TK_OPENBR){
		cout<<"resta -> [elist]"<<endl;
		elist();
		w=getNextWord();
		if(w.code!=TK_CLOSEBR)
			error();
	}
	else{
		rollBack(w);
		attr.place=inArray;
	 	cout<<"resta -> $"<<endl;
	}
	return attr;
}

void elist(){
	cout<<"elist -> expr rest1"<<endl;
	expr();
	rest1();
}

void rest1(){
	Word w=getNextWord();
	if(w.code==TK_COMMA){
		cout<<"rest1 -> , expr rest1"<<endl;
		expr();
		rest1();
	}else{
		rollBack(w);
		cout<<"rest1 -> $"<<endl;
	}
}


//----------------------------------------------------------
//语法分析器入口
void parsing(){
	stmts();//做语法分析
	success();
	SemanticAnalyzer();//做语义分析
}

//--------------------------------------------------------------------------------------------


int main(){

	string filename;		//文件路径
	cout << "input your filePath：" << endl;
	while (true) {
		cin >> filename;		//读取文件路径
		if ((fopen_s(&fp,filename.c_str(), "r"))==0)		//打开文件
			break;
		else
			cout << "FilePath Error！" << endl;	//读取失败
	}

	parsing();

	fclose(fp);
}


