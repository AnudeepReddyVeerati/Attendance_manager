#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STUDENTS 2
#define MAX_NAME_LENGTH 50
#define MAX_DATE_LENGTH 20
#define CLASSES_PER_DAY 8

typedef struct {
    char date[MAX_DATE_LENGTH];
    int classes_attended;
    int is_holiday;
} AttendanceRecord;

typedef struct {
    char name[MAX_NAME_LENGTH];
    char filename[MAX_NAME_LENGTH];
} Student;

void initializeStudents(Student students[]);
int isHoliday(int day, int month, int year);
int getSaturdayType(int day, int month, int year);
void markAttendance(Student student);
void viewAttendance(Student student);
void displayMenu();
void createStudentFiles(Student students[]);
int countRecords(char* filename);
void calculateAttendanceStats(char* filename, int* total_days, int* total_classes, int* attended_classes, float* percentage);

int main() {
    Student students[MAX_STUDENTS];
    int choice, student_choice;
    
    initializeStudents(students);
    createStudentFiles(students);
    
    printf("=== STUDENT ATTENDANCE MANAGEMENT SYSTEM ===\n");
    printf("Students: %s, %s\n", students[0].name, students[1].name);
    printf("Holiday Policy: 2nd & 4th Saturdays, All Sundays\n\n");
    
    while(1) {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                printf("\nSelect Student:\n");
                printf("1. %s\n", students[0].name);
                printf("2. %s\n", students[1].name);
                printf("Enter choice (1-2): ");
                scanf("%d", &student_choice);
                
                if(student_choice >= 1 && student_choice <= 2) {
                    markAttendance(students[student_choice - 1]);
                } else {
                    printf("Invalid student selection!\n");
                }
                break;
                
            case 2:
                printf("\nSelect Student to View Attendance:\n");
                printf("1. %s\n", students[0].name);
                printf("2. %s\n", students[1].name);
                printf("Enter choice (1-2): ");
                scanf("%d", &student_choice);
                
                if(student_choice >= 1 && student_choice <= 2) {
                    viewAttendance(students[student_choice - 1]);
                } else {
                    printf("Invalid student selection!\n");
                }
                break;
                
            case 3:
                printf("\nThank you for using the Attendance Management System!\n");
                exit(0);
                
            default:
                printf("Invalid choice! Please try again.\n");
        }
        
        printf("\nPress Enter to continue...");
        getchar();
        getchar();
    }
    
    return 0;
}

void initializeStudents(Student students[]) {
    strcpy(students[0].name, "Anudeep");
    strcpy(students[0].filename, "anudeep_attendance.txt");
    
    strcpy(students[1].name, "Jasprit");
    strcpy(students[1].filename, "jasprit_attendance.txt");
}

void createStudentFiles(Student students[]) {
    FILE *file;
    for(int i = 0; i < MAX_STUDENTS; i++) {
        file = fopen(students[i].filename, "a");
        if(file != NULL) {
            fclose(file);
        }
    }
}

int isHoliday(int day, int month, int year) {
    struct tm date = {0};
    date.tm_year = year - 1900;
    date.tm_mon = month - 1;
    date.tm_mday = day;
    
    mktime(&date);
    
    int weekday = date.tm_wday;
    
    if(weekday == 0) {
        return 1;
    }
    
    if(weekday == 6) {
        int saturday_type = getSaturdayType(day, month, year);
        if(saturday_type == 2 || saturday_type == 4) {
            return 1;
        }
    }
    
    return 0;
}

int getSaturdayType(int day, int month, int year) {
    int saturday_count = 0;
    
    for(int i = 1; i <= day; i++) {
        struct tm date = {0};
        date.tm_year = year - 1900;
        date.tm_mon = month - 1;
        date.tm_mday = i;
        
        mktime(&date);
        
        if(date.tm_wday == 6) {
            saturday_count++;
        }
    }
    
    return saturday_count;
}

void markAttendance(Student student) {
    FILE *file;
    time_t now;
    struct tm *local_time;
    char date_str[MAX_DATE_LENGTH];
    int classes_attended;
    
    time(&now);
    local_time = localtime(&now);
    
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", local_time);
    
    int day = local_time->tm_mday;
    int month = local_time->tm_mon + 1;
    int year = local_time->tm_year + 1900;
    
    printf("\n=== MARKING ATTENDANCE FOR %s ===\n", student.name);
    printf("Date: %s\n", date_str);
    
    if(isHoliday(day, month, year)) {
        printf("Today is a HOLIDAY!\n");
        printf("Holiday Reason: ");
        
        struct tm date = {0};
        date.tm_year = year - 1900;
        date.tm_mon = month - 1;
        date.tm_mday = day;
        mktime(&date);
        
        if(date.tm_wday == 0) {
            printf("Sunday\n");
        } else if(date.tm_wday == 6) {
            int sat_type = getSaturdayType(day, month, year);
            if(sat_type == 2) {
                printf("Second Saturday\n");
            } else if(sat_type == 4) {
                printf("Fourth Saturday\n");
            }
        }
        
        file = fopen(student.filename, "a");
        if(file != NULL) {
            fprintf(file, "%s,0,1\n", date_str);
            fclose(file);
            printf("Holiday recorded successfully!\n");
        } else {
            printf("Error: Could not open file!\n");
        }
        return;
    }
    
    printf("Enter number of classes attended today (0-%d): ", CLASSES_PER_DAY);
    scanf("%d", &classes_attended);
    
    if(classes_attended < 0 || classes_attended > CLASSES_PER_DAY) {
        printf("Invalid input! Classes should be between 0 and %d.\n", CLASSES_PER_DAY);
        return;
    }
    
    file = fopen(student.filename, "a");
    if(file != NULL) {
        fprintf(file, "%s,%d,0\n", date_str, classes_attended);
        fclose(file);
        printf("Attendance marked successfully for %s!\n", student.name);
        printf("Classes attended: %d/%d\n", classes_attended, CLASSES_PER_DAY);
    } else {
        printf("Error: Could not save attendance!\n");
    }
}

void viewAttendance(Student student) {
    FILE *file;
    char line[100];
    char date[MAX_DATE_LENGTH];
    int classes_attended, is_holiday;
    int total_days = 0, total_classes = 0, attended_classes = 0;
    int working_days = 0, holidays = 0;
    float attendance_percentage;
    
    printf("\n=== ATTENDANCE REPORT FOR %s ===\n", student.name);
    printf("%-12s %-15s %-10s %-10s\n", "Date", "Classes", "Total", "Status");
    printf("================================================\n");
    
    file = fopen(student.filename, "r");
    if(file == NULL) {
        printf("No attendance records found for %s!\n", student.name);
        return;
    }
    
    while(fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%d,%d", date, &classes_attended, &is_holiday);
        
        total_days++;
        
        if(is_holiday) {
            printf("%-12s %-15s %-10s %-10s\n", date, "N/A", "N/A", "HOLIDAY");
            holidays++;
        } else {
            printf("%-12s %-15d %-10d %-10s\n", date, classes_attended, CLASSES_PER_DAY, 
                   (classes_attended >= CLASSES_PER_DAY * 0.75) ? "GOOD" : "POOR");
            working_days++;
            attended_classes += classes_attended;
            total_classes += CLASSES_PER_DAY;
        }
    }
    fclose(file);
    
    if(total_classes > 0) {
        attendance_percentage = ((float)attended_classes / total_classes) * 100;
    } else {
        attendance_percentage = 0;
    }
    
    printf("================================================\n");
    printf("ATTENDANCE SUMMARY:\n");
    printf("Total Days Recorded: %d\n", total_days);
    printf("Working Days: %d\n", working_days);
    printf("Holidays: %d\n", holidays);
    printf("Total Classes Conducted: %d\n", total_classes);
    printf("Classes Attended: %d\n", attended_classes);
    printf("Attendance Percentage: %.2f%%\n", attendance_percentage);
    printf("================================================\n");
    
    if(attendance_percentage >= 75) {
        printf("STATUS: EXCELLENT ATTENDANCE! ✓\n");
    } else if(attendance_percentage >= 60) {
        printf("STATUS: GOOD ATTENDANCE\n");
    } else if(attendance_percentage >= 45) {
        printf("STATUS: AVERAGE ATTENDANCE - NEEDS IMPROVEMENT\n");
    } else {
        printf("STATUS: POOR ATTENDANCE - CRITICAL! ✗\n");
    }
}

void displayMenu() {
    printf("\n=== MENU ===\n");
    printf("1. Mark Attendance\n");
    printf("2. View Attendance Report\n");
    printf("3. Exit\n");
    printf("============\n");
}

int countRecords(char* filename) {
    FILE *file = fopen(filename, "r");
    int count = 0;
    char line[100];
    
    if(file == NULL) return 0;
    
    while(fgets(line, sizeof(line), file)) {
        count++;
    }
    
    fclose(file);
    return count;
}