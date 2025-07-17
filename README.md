# Minishell Builtin Commands Documentation

## Genel Yapı

### Environment List Yapısı
```c
typedef struct s_env {
    char *key;        // Environment değişkeninin adı (örn: "PATH")
    char *value;      // Environment değişkeninin değeri (örn: "/usr/bin:/bin")
    struct s_env *next; // Bir sonraki node'a pointer (linked list)
} t_env;
```

Bu yapı tüm environment değişkenlerini linked list olarak tutar. Her node bir anahtar-değer çiftini saklar.

## Fonksiyonlar

### 1. `builtin_export(char **args, t_env *env_list)`
**Amaç:** Bash'daki `export` komutunun basit versiyonu  
**Parametre:** 
- `args`: Komut argümanları (args[0] = "export", args[1] = NULL)
- `env_list`: Environment listesi

**Davranış:**
- Argüman yoksa tüm environment değişkenlerini `declare -x KEY="VALUE"` formatında yazdırır
- Bu sadece görüntüleme işlemi yapar, yeni değişken eklemez
- Return değeri: 0 (başarılı)

### 2. `builtin_unset(char **args, t_env **env_list)`
**Amaç:** Environment değişkenini silme  
**Parametre:**
- `args`: Komut argümanları (args[0] = "unset", args[1] = silinecek_değişken_adı)
- `env_list`: Environment listesinin pointer'ı (çift pointer çünkü liste değişebilir)

**Davranış:**
- Belirtilen environment değişkenini listeden bulur ve siler
- Memory'yi temizler (free)
- Liste başını güncelleyebilir (bu yüzden double pointer)
- Return değeri: 0

### 3. `builtin_env(char **args, t_env *env_list)`
**Amaç:** Tüm environment değişkenlerini gösterme  
**Parametre:**
- `args`: Kullanılmıyor (void casting yapılmış)
- `env_list`: Environment listesi

**Davranış:**
- Tüm environment değişkenlerini `KEY=VALUE` formatında yazdırır
- Hiçbir argüman almaz
- Return değeri: 0

## Yardımcı Fonksiyonlar

### `find_env_var(t_env *env_list, char *key)`
- Belirtilen anahtarı listede arar
- Bulursa node'u döndürür, bulamazsa NULL döndürür
- String karşılaştırma için `strcmp` kullanır

### `init_env_list(char **environ)`
- Sistem environment'ından yeni bir linked list oluşturur
- `environ` dizisini döngüyle gezer
- Her environment değişkenini '=' karakterinden böler
- Yeni node'lar oluşturup listeye ekler
- Return değeri: Oluşturulan listenin başı

### `get_env_value(t_env *env_list, char *key)`
- Önce kendi listemizde arar
- Bulamazsa sistem `getenv()` ile kontrol eder
- Değeri bulursa string döndürür, bulamazsa NULL

## Kullanım Örneği

```c
// Environment listesini başlat
t_env *my_env = init_env_list(environ);

// Tüm environment değişkenlerini göster
char *env_args[] = {"env", NULL};
builtin_env(env_args, my_env);

// Export formatında göster
char *export_args[] = {"export", NULL};
builtin_export(export_args, my_env);

// PATH değişkenini sil
char *unset_args[] = {"unset", "PATH", NULL};
builtin_unset(unset_args, &my_env);
```

## Önemli Notlar

### Memory Management
- `init_env_list` ile oluşturulan her node malloc ile ayrılır
- `unset` fonksiyonu memory'yi free eder
- Program sonunda tüm liste temizlenmelidir

### String Fonksiyonları
- `strchr()`: '=' karakterini bulmak için
- `strndup()`: String'in bir kısmını kopyalamak için
- `strdup()`: String'i tamamen kopyalamak için
- `strcmp()`: String karşılaştırma için

### Derleme
```bash
gcc -o minishell_test minishell_builtins.c
./minishell_test
```

### Test Senaryoları
1. `env` komutu ile tüm değişkenleri görüntüle
2. `export` komutu ile export formatında görüntüle
3. `unset VARIABLE_NAME` ile değişken sil
4. Tekrar `env` ile silindiğini kontrol et

Bu implementasyon minishell projesi için temel builtin komutları sağlar ve bash davranışını taklit eder.
