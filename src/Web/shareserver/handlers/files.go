package handlers

import (
	"encoding/json"
	"net/http"
	"shareserver/models"
	"time"
)

var files []models.File

func SubmitFile(w http.ResponseWriter, r *http.Request) {
	var file models.File
	err := json.NewDecoder(r.Body).Decode(&file)
	if err != nil {
		return
	}
	file.SubmissionDate = time.Now()
	file.Version = 1
	files = append(files, file)
	w.WriteHeader(http.StatusCreated)
}

func DownloadFile(w http.ResponseWriter, r *http.Request) {
	// Implement file download logic here
}
