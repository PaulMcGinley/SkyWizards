package models

import "time"

type File struct {
	ID             int
	Title          string
	FileSize       int64
	CreatedBy      int
	SubmissionDate time.Time
	TotalDownloads int
	Version        int
	Images         [3]string
}
