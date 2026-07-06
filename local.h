#ifndef LOCAL_H
#define LOCAL_H

/* Language definition constants */
#define LANG_EN 1
#define LANG_ES 2
#define LANG_JA 3

/* * Smart inclusion: If __has_include is supported, check if config.h exists.
 * This prevents compilation crashes before running mconfig for the first time.
 */
#if defined(__has_include)
    #if __has_include("config.h")
        #include "config.h"
    #endif
#else
    #include "config.h"
#endif

/* Fallback default if config.h hasn't been generated yet by mconfig */
#ifndef CURRENT_LANG
    #define CURRENT_LANG LANG_EN
#endif

#if CURRENT_LANG == LANG_ES
    #define MSG_PASS_PROMPT "[froot] contraseña: "
    #define MSG_PASS_RETRY  "Lo siento, intenta de nuevo.\n"
    #define MSG_BRUTE_FORCE "froot error: Demasiados fallos de autenticación. Terminal bloqueada por seguridad.\n"
    #define MSG_USAGE       "Uso: %s <comando> [argumentos...]\n"
    #define MSG_UNAUTHORIZED "froot error: Ejecución no autorizada.\n"
    #define MSG_AUTH_FAIL   "froot error: Fallo de autenticación.\n"
    #define MSG_JAIL_FAIL   "froot error: Fallo al entrar en el entorno chroot"
    #define MSG_EXEC_FAIL   "froot error: Ejecución del comando fallida"
#elif CURRENT_LANG == LANG_JA
    #define MSG_PASS_PROMPT "[froot] パスワード: "
    #define MSG_PASS_RETRY  "申し訳ありません、もう一度お試しください。\n"
    #define MSG_BRUTE_FORCE "froot エラー: 認証失敗が多すぎます。セキュリティのため端末をロックします。\n"
    #define MSG_USAGE       "使用法: %s <コマンド> [引数...]\n"
    #define MSG_UNAUTHORIZED "froot エラー: 承認されていないコマンド実行です。\n"
    #define MSG_AUTH_FAIL   "froot エラー: 認証に失敗しました。\n"
    #define MSG_JAIL_FAIL   "froot エラー: chroot環境への移行に失敗しました"
    #define MSG_EXEC_FAIL   "froot エラー: コマンドの実行に失敗しました"
#else /* Default: LANG_EN */
    #define MSG_PASS_PROMPT "[froot] password: "
    #define MSG_PASS_RETRY  "Sorry, try again.\n"
    #define MSG_BRUTE_FORCE "froot error: Too many authentication failures. Terminal locked for security mitigating measures.\n"
    #define MSG_USAGE       "Usage: %s <command> [arguments...]\n"
    #define MSG_UNAUTHORIZED "froot error: Unauthorized command execution.\n"
    #define MSG_AUTH_FAIL   "froot error: Authentication failure.\n"
    #define MSG_JAIL_FAIL   "froot error: Failed to enter chroot jail restriction scope"
    #define MSG_EXEC_FAIL   "froot error: Command execution failed"
#endif

#endif
