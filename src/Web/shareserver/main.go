package main

import (
	"net/http"
	"shareserver/handlers"
)

func main() {
	http.HandleFunc("/register", handlers.Register)
	http.HandleFunc("/login", handlers.Login)
	http.HandleFunc("/submit", handlers.SubmitFile)
	http.HandleFunc("/download", handlers.DownloadFile)

	http.ListenAndServe(":8080", nil)
}
