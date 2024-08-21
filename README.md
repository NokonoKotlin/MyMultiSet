## MyMultiSet (平衡二分木)

C++ 使いの競プロ er なら誰しもが思う「`std::set` にもっと機能を付けてくれ〜」を解決したデータ構造です。
(`std::set` との互換性は全くありません)

#### 使用例
```cpp
#include<iostream>
#include "MyMultiSet.hpp"

using std::cout ,std::endl , std::cin;

// ABC281-E (https://atcoder.jp/contests/abc281/tasks/abc281_e)
int main(){
    int n , m , k;cin >> n >> m >> k;
    long long A[200002];
    MyMultiSet<long long,long long> S;
    for(int i = 0 ; i < n ; i++)cin >> A[i];

    for(int i = 0 ; i < m ; i++)S.insert(A[i]);
    
    cout << S.GetRange(0,k).Sum_key << " ";
    for(int i = 1 ; i < n-m+1 ; i++ ){
        S.erase(A[i-1]);
        S.insert(A[i+m-1]);
        cout << S.GetRange(0,k).Sum_key << " ";
    }
    cout << endl;
    return 0;
}
```

## ライセンス (license)
Copyright (c) 2024 NokonoKotlin Released under the MIT license(https://opensource.org/licenses/mit-license.php)


## NokonoKotlin のお気持ち
- ライブラリに組み込むのは自由だけど、自分のライブラリと称するのはやめてほしいよ。


### 概要
`type_key` 型の `Key` と `type_value` 型の `Value` を持つ**順序付き集合**で、(Key , Value) が**辞書順**にソートされている。ただし、Value を無視する場合は辞書順ではなく、`Key` の順序でソートされる。
- C++ の `std::set` とは異なり、`Key` の重複を許す ( `Value` も当然重複 OK )。
- `get(i)` や `[i]` で `i` 番目のノードのコピーを 0-index で取得。ただし隣接頂点へのアクセス (ポインタ) が封印されたものを返す。
- `Delete(i)` で小さい順で `i` 番目の要素を削除する。
- `GetRange(l,r)` は要素の辞書順の半開区間 `[l,r)` をカバーする部分木の根のコピーを返す。`get()` 同様に、隣接頂点のポインタは封印されている。
    - `GetRange(l,r).Sum_val` のようにして `[l,r)` の持つ要素のモノイド積を取得する

#### (Key,Value) に関して以下の操作が可能
- `insert_pair(k,v)`  
    - `(k,v)` を持つノードを追加
- `erase_pair(k,v)`
    - `(k,v)` を持つノードを(存在すれば)削除する。
- `upper_bound_pair(k,v)` 
    - `(Key,Value)` が辞書順で `(k,v)` 以下の要素数を返す
- `lower_bound_pair(k,v)`  
    - `(Key,Value)` が辞書順で `(k,v)` 未満の要素数を返す
- `find_pair(k,v)` 
    - `(Key,Value)` が `(k,v)` である要素の index を返す。存在しなければ `-1` を返す (0-index)。

#### ノードの Key だけに注目して、通常の set のように振る舞わせることもできる。  
- `insert(k)`
    - `Key = k` である要素を追加する。ただし、`Value` を指定しないので `Value` は未定義とする。
- `erase(k)` 
    - `Key = k` である要素を一つ削除する。ただし、`Value` について特に指定しないことに注意。
- `upper_bound(k)` 
    - `Key` が `k` 以下の要素数を返す
- `lower_bound(k)`
    - `Key` が `k` 未満の要素数を返す
- `find(k)`
    - `Key` が `k` である要素の index を返す。存在しなければ `-1` を返す (0-index)。

#### ペアを持つノードとKeyしか持たないノードが混在するといけない
- `insert()` を呼び出した時点で、`Value` が未定義の要素が存在することになる
    - `insert()` を呼び出した後は `upper_bound_pair` , `find_pair` , `RangeValueMaxQuery` など、`Value` に関する関数を呼び出すとランタイムエラーになるようにしました。(ただし、`Key` だけに関係する関数は変わらず使用できる。

#### その他
- CompareNode() を変更することで要素の並び順を変更できる。
- 必要があれば `eval()` に遅延評価を実装して良い。
    - 順序付きなので、評価した後に順序が崩れないように制約をつける必要がある。
