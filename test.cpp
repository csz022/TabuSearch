#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

// 物品結構
struct item {
    int profit;  // 物品的價值
    int weight;  // 物品的重量
};

//計算總利潤
int calculateprofit(const vector<int>& solution, const vector<item>& items) {
    int totalprofit = 0;
    for (size_t i = 0; i < solution.size(); i++) {
        if (solution[i] == 1) {  // 如果物品被選中
            totalprofit += items[i].profit;
        }
    }
    return totalprofit;
}

//計算總重量
int calculateweight(const vector<int>& solution, const vector<item>& items) {
    int totalweight = 0;
    for (size_t i = 0; i < solution.size(); i++) {
        if (solution[i] == 1) {  // 如果物品被選中
            totalweight += items[i].weight;
        }
    }
    return totalweight;
}

//生成初始解，按照性價比（profit/weight）排序選擇物品
vector<int> initial(int N, int W, const vector<item>& items) {
    // 計算每個物品的性價比並存儲索引
    vector<pair<double, int>> ratio(N);
    for (int i = 0; i < N; i++) {
        ratio[i] = {(double)items[i].profit / items[i].weight, i};  // 計算性價比
    }
    // 按性價比從大到小排序
    sort(ratio.begin(), ratio.end(), greater<pair<double, int>>());
    
    vector<int> solution(N, 0);  // 初始化解向量，全部設為0
    int currentweight = 0;  // 當前背包重量
    
    // 依照性價比由高到低嘗試放入物品
    for (const auto& r : ratio) {
        int i = r.second;  // 獲取物品索引
        if (currentweight + items[i].weight <= W) {  // 如果加入此物品不超重
            solution[i] = 1;  // 選擇該物品
            currentweight += items[i].weight;  // 更新當前重量
        }
    }
    return solution;
}

//將解向量轉換為字符串，用於tabu list比較
string stringsolution(const vector<int>& solution) {
    string result;
    for (int i : solution) {
        result += to_string(i);  // 將每個位轉換為字符
    }
    return result;
}

//產生鄰域解，通過翻轉單個位元來生成新解
vector<int> tweak(const vector<int>& solution, int W, const vector<item>& items) {
    vector<int> newsolution = solution;
    int N = solution.size();
    
    //嘗試翻轉每一位，找到第一個可行解就返回
    for (size_t i = 0; i < N; i++) {
        newsolution[i] = 1 - newsolution[i];  // 翻轉當前位
        if (calculateweight(newsolution, items) <= W) {  // 檢查是否可行
            return newsolution;
        }
        newsolution[i] = solution[i];  // 不可行則恢復
    }
    
    return solution;  // 如果找不到可行解，返回原解
}

//tabu search
vector<int> tabusearch(int N, int W, const vector<item>& items, int tabuLength, int maxIterations) {
    // 初始化best solution和current solution
    vector<int> bestsolution = initial(N, W, items);
    int bestprofit = calculateprofit(bestsolution, items);
    
    vector<int> currentsolution = bestsolution;
    int currentprofit = bestprofit;
    
    // 初始化tabu list
    vector<string> tabulist; 
    
    // 迭代循環
    for (size_t iter = 0; iter < maxIterations; iter++) {
        // 生成new solution
        vector<int> newsolution = tweak(currentsolution, W, items);
        int newprofit = calculateprofit(newsolution, items);
        string newstringsolution = stringsolution(newsolution);
        
        // 檢查是否在tabu list中
        bool istabu = find(tabulist.begin(), tabulist.end(), newstringsolution) != tabulist.end();
        
        // 如果不在tabu list中或new profit大於best profit，則更新current solution  
        if (!istabu || newprofit > bestprofit) {
            currentsolution = newsolution;
            currentprofit = newprofit;
            
            // 更新tabu list
            tabulist.push_back(newstringsolution);
            if (tabulist.size() > tabuLength) {
                tabulist.erase(tabulist.begin());  // 移除最舊的禁忌解
            }
            
            // 更新best solution
            if (currentprofit > bestprofit) {
                bestsolution = currentsolution;
                bestprofit = currentprofit;
            }
        }
    }
    
    return bestsolution;
}


int main() {
    srand(time(0));  // 初始化隨機數生成器
    
    // input
    int N, W;  // N為物品數量，W為背包容量
    cin >> N >> W;
    vector<item> items(N);
    for (int i = 0; i < N; i++) {
        cin >> items[i].weight >> items[i].profit;  // 讀取每個物品的重量和價值
    }
    
    // 執行禁忌搜索，設置禁忌表長度為20，最大迭代次數為1000
    vector<int> bestsolution = tabusearch(N, W, items, 20, 1000);
    int maxprofit = calculateprofit(bestsolution, items);
    
    // output
    cout << "max profit: " << maxprofit << "\n";
    cout << "solution: ";
    for (int bit : bestsolution) {
        cout << bit;
    }
    cout << endl;
    
    return 0;
}
