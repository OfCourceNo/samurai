#include<iostream>
#include<vector>
using std::vector;
using std::cout;
using std::endl;

typedef vector<int> vi;
typedef vector<vector<int>> vvi;



//field情報の出力関数
void vv_print(vvi v){
	for (int i = 0; i < 15; i++){
		for (int j = 0; j < 15; j++){
			cout << v[i][j] ;
		}
		cout << endl;
	}	
}

//二次元配列の０初期化
void init(vvi &v){
	v = vvi(15, vi(15,0));
}

//0check
bool check0(vvi v){
	for (int i = 0 ;i < 15; i++){
		for (int j = 0; j < 15; j++){
			if(v[i][j] > 0){
				return false;
			}
		}
	}
	return true;
}
//二次元ベクトルの90度回転
void rotation(int &x,int &y,int n){
	for (int i = 0; i < n; i++){
		int tmpx = x;
		int tmpy = y;
		x = (-1)*tmpy;
		y = tmpx;
	}
}
//masakriの攻撃パターン
void masakari(int px,int py,vvi &tmp,int n){
	//masakariの座標設定。
	int dx[7] = {-1, 0, 1, -1, 1, -1, 1};
	int dy[7] = {-1, -1, -1, 0, 0, 1, 1};
	for (int j = 0 ; j < 7; j++){
		int tmpx = dx[j],tmpy = dy[j];
		rotation(tmpx,tmpy,n);
		tmpx += px;
		tmpy += py;
		if (0 <= tmpx and tmpx < 15 and 0 <= tmpy and tmpy < 15 and tmp[tmpx][tmpy] == 1){
			tmp[tmpx][tmpy] = 0;
		}
	}
}

void katana(int px,int py,vvi &tmp,int n){
	//katanaの座標設定。
	int dx[6] = {-1, 0, 1, 1, 0};
	int dy[6] = { 1, 1,-1, 0, 0};
	for (int j = 0 ; j < 5; j++){
		int tmpx = dx[j],tmpy = dy[j];
		rotation(tmpx,tmpy,n);
		tmpx += px;
		tmpy += py;
		if (0 <= tmpx and tmpx < 15 and 0 <= tmpy and tmpy < 15 and tmp[tmpx][tmpy] == 1){
			tmp[tmpx][tmpy] = 0;
		}
	}
}

void yari(int px,int py,vvi &tmp,int n){
	//katanaの座標設定。
	int dx[4] = {0, 0, 0, 0};
	int dy[4] = {1, 2, 3, 4};
	for (int j = 0 ; j < 4; j++){
		int tmpx = dx[j],tmpy = dy[j];
		rotation(tmpx,tmpy,n);
		tmpx += px;
		tmpy += py;
		if (0 <= tmpx and tmpx < 15 and 0 <= tmpy and tmpy < 15 and tmp[tmpx][tmpy] == 1){
			tmp[tmpx][tmpy] = 0;
		}
	}
}

//masakariの存在場所列挙。existに結果格納
void masakari_check(vvi chlo,vvi &exist){
	vvi tmp(15,vi(15));
	init(tmp);
	int pointx = 0, pointy = 0;
	for (int rot  = 0; rot < 4; rot++){
		for (int pointx = 0; pointx < 15; pointx++){
			for (int pointy = 0; pointy < 15; pointy++){
				int adx[3] = {1, -1, 0};
				int ady[3] = {0, 0, -1};
				tmp = chlo;
				masakari(pointx,pointy,tmp,rot);
				//とりあえず加算して見た。多分数字多ければ可能性も高い
				if (check0(tmp)){
					for (int i = 0; i < 3; i++){
						rotation(adx[i],ady[i],rot);
						if (0 <= pointx+adx[i] and pointx + adx[i] < 15 and 0 <= pointy+ady[i] and pointy + ady[i] < 15){
						exist[pointx+adx[i]][pointy+ady[i]] += 1;
						}
					}
				}
			}
		}

	}
}

void katana_check(vvi chlo,vvi &exist){
	vvi tmp(15,vi(15));
	int pointx = 0, pointy = 0;
	for (int rot = 0; rot < 4; rot++){
		for (int pointx = 0; pointx < 15; pointx++){
			for (int pointy = 0; pointy < 15; pointy++){
				int adx[2] = {0, 1};
				int ady[2] = {1, 0};
				tmp = chlo;
				katana(pointx,pointy,tmp,rot);
				//とりあえず加算して見た。多分数字多ければ可能性も高い
				if (check0(tmp)){
					for (int i = 0; i < 2; i++){
						rotation(adx[i],ady[i],rot);
						if (0 <= pointx+adx[i] and pointx + adx[i] < 15 and 0 <= pointy+ady[i] and pointy + ady[i] < 15){
						exist[pointx+adx[i]][pointy+ady[i]] += 1;
						}
					}
				}
			}
		}

	}

}


void yari_check(vvi chlo,vvi &exist){
	vvi tmp(15,vi(15));
	int pointx = 0, pointy = 0;
	for (int rot = 0; rot < 4; rot++){
		for (int pointx = 0; pointx < 15; pointx++){
			for (int pointy = 0; pointy < 15; pointy++){
				int adx[1] = {0};
				int ady[1] = {1};
				tmp = chlo;
				yari(pointx,pointy,tmp,rot);
				//とりあえず加算して見た。多分数字多ければ可能性も高い
				if (check0(tmp)){
					for (int i = 0; i < 1; i++){
						rotation(adx[i],ady[i],rot);
						if (0 <= pointx+adx[i] and pointx + adx[i] < 15 and 0 <= pointy+ady[i] and pointy + ady[i] < 15){
						exist[pointx+adx[i]][pointy+ady[i]] += 1;
						}
					}
				}
			}
		}
	}
}

int main(){
	vvi change_locate(15,vi(15));
	init(change_locate);
	vvi exist(15,vi(15));
	init(exist);

	//change_locate[10][10] = 1; 
	//change_locate[10][11] = 1; 
	//change_locate[9][11] = 1; 
	//change_locate[11][9] = 1; 
	//change_locate[11][10] = 1; 
	change_locate[0][0] = 1; 
	masakari_check(change_locate,exist);
	//katana_check(change_locate,exist);
	//yari_check(change_locate,exist);
	cout << "change_loacte" << endl;
	vv_print(change_locate);
	cout << "exist" << endl;
	vv_print(exist);
	return 0;
}
