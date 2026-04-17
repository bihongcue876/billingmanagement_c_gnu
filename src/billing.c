#include "billing.h"
#include "fileio.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static BillingStandard standards[MAX_PLANS];
static int standard_count = 0;

void load_standards(void) {
    standard_count = 0;
    load_billing(standards, &standard_count);
    if (standard_count == 0) {
        strcpy(standards[0].planId, "1");
        strcpy(standards[0].planName, "普通");
        standards[0].segmentCount = 3;
        standards[0].segments[0].startHour = 0;
        standards[0].segments[0].endHour = 1;
        standards[0].segments[0].pricePerHour = 1.8;
        
        standards[0].segments[1].startHour = 1;
        standards[0].segments[1].endHour = 3;
        standards[0].segments[1].pricePerHour = 1.2;
        
        standards[0].segments[2].startHour = 3;
        standards[0].segments[2].endHour = -1;
        standards[0].segments[2].pricePerHour = 0.6;
        standards[0].nDel = 0;
        standard_count = 1;
        save_billing(standards, standard_count);
    }
}

double calculate_fee(double hours) {
    load_standards();
    
    if (standard_count == 0) return 0;
    
    BillingStandard* std = &standards[0];
    double total = 0;
    for (int i = 0; i < std->segmentCount; i++) {
        double start = std->segments[i].startHour;
        double end = std->segments[i].endHour;
        double price = std->segments[i].pricePerHour;
        
        if (end < 0) {
            if (hours > start) {
                total += (hours - start) * price;
            }
            break;
        } else {
            if (hours > start) {
                double segmentHours = (hours > end) ? (end - start) : (hours - start);
                total += segmentHours * price;
            }
        }
    }
    
    return total;
}

static void show_standard(BillingStandard* std) {
    printf("套餐ID: %s\n", std->planId);
    printf("套餐名称: %s\n", std->planName);
    printf("段号\t起始(小时)\t终止(小时)\t单价(元/小时)\n");
    for (int i = 0; i < std->segmentCount; i++) {
        char endStr[20];
        if (std->segments[i].endHour < 0) {
            strcpy(endStr, "无限制");
        } else {
            sprintf(endStr, "%.1f", std->segments[i].endHour);
        }
        printf("%d\t%.1f\t\t%s\t\t%.2f\n",
               i + 1,
               std->segments[i].startHour,
               endStr,
               std->segments[i].pricePerHour);
    }
}

void billing_menu(void) {
    load_standards();
    
    while (1) {
        printf("\n=== 计费标准设置 ===\n");
        printf("1. 查看套餐列表\n");
        printf("2. 新增套餐\n");
        printf("3. 修改套餐\n");
        printf("4. 删除套餐\n");
        printf("5. 设置默认套餐\n");
        printf("0. 返回\n");
        
        int choice = get_choice(0, 5);
        if (choice == 0) break;
        
        char planId[32];
        
        switch (choice) {
            case 1:
                printf("\n=== 套餐列表 ===\n");
                for (int i = 0; i < standard_count; i++) {
                    if (standards[i].nDel == 0) {
                        printf("\n--- 套餐 %d ---\n", i + 1);
                        show_standard(&standards[i]);
                    }
                }
                break;
                
            case 2:
                printf("\n新增套餐\n");
                if (standard_count >= MAX_PLANS) {
                    printf("套餐已满！\n");
                    break;
                }
                
                get_string("请输入套餐ID: ", planId, sizeof(planId));
                {
                    int exists = 0;
                    for (int i = 0; i < standard_count; i++) {
                        if (standards[i].nDel == 0 && strcmp(standards[i].planId, planId) == 0) {
                            exists = 1;
                            break;
                        }
                    }
                    if (exists) {
                        printf("套餐ID已存在！\n");
                        break;
                    }
                }
                
                BillingStandard* newStd = &standards[standard_count];
                strcpy(newStd->planId, planId);
                get_string("请输入套餐名称: ", newStd->planName, sizeof(newStd->planName));
                
                printf("请输入分段数: ");
                int n = get_int("");
                
                if (n < 1 || n > MAX_SEGMENTS) {
                    printf("段数必须在1-%d之间！\n", MAX_SEGMENTS);
                    break;
                }
                
                newStd->segmentCount = n;
                
                for (int i = 0; i < n; i++) {
                    printf("\n第%d段：\n", i + 1);
                    
                    if (i == 0) {
                        printf("起始小时（必须是0）: ");
                        double start = 0;
                        scanf("%lf", &start);
                        getchar();
                        if (start != 0) {
                            printf("第一段起始必须是0！\n");
                            i--;
                            continue;
                        }
                        newStd->segments[i].startHour = start;
                    } else {
                        newStd->segments[i].startHour = newStd->segments[i-1].endHour;
                        printf("起始小时（自动继承上段终止）: %.1f\n", newStd->segments[i].startHour);
                    }
                    
                    if (i == n - 1) {
                        printf("终止小时（输入-1表示无限制）: ");
                        double end = -1;
                        scanf("%lf", &end);
                        getchar();
                        if (end != -1) {
                            printf("最后一段终止必须是-1！\n");
                            i--;
                            continue;
                        }
                        newStd->segments[i].endHour = end;
                    } else {
                        printf("终止小时（不能是-1）: ");
                        double end = 0;
                        scanf("%lf", &end);
                        getchar();
                        if (end == -1) {
                            printf("非最后一段不能输入-1！\n");
                            i--;
                            continue;
                        }
                        if (end <= newStd->segments[i].startHour) {
                            printf("终止小时必须大于起始小时！\n");
                            i--;
                            continue;
                        }
                        newStd->segments[i].endHour = end;
                    }
                    
                    printf("该段单价（元/小时）: ");
                    double price = 0;
                    scanf("%lf", &price);
                    getchar();
                    if (price <= 0) {
                        printf("价格必须大于0！\n");
                        i--;
                        continue;
                    }
                    newStd->segments[i].pricePerHour = price;
                }
                
                newStd->nDel = 0;
                standard_count++;
                save_billing(standards, standard_count);
                printf("新增成功！\n");
                break;
                
            case 3:
                printf("\n修改套餐\n");
                get_string("请输入要修改的套餐ID: ", planId, sizeof(planId));
                {
                    int found = -1;
                    for (int i = 0; i < standard_count; i++) {
                        if (standards[i].nDel == 0 && strcmp(standards[i].planId, planId) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found == -1) {
                        printf("套餐不存在！\n");
                        break;
                    }
                    
                    printf("\n当前套餐信息：\n");
                    show_standard(&standards[found]);
                    
                    printf("\n请选择修改项：\n");
                    printf("1. 修改套餐名称\n");
                    printf("2. 重新设置分段\n");
                    printf("0. 返回\n");
                    
                    int mod = get_choice(0, 2);
                    if (mod == 0) break;
                    
                    if (mod == 1) {
                        get_string("请输入新套餐名称: ", standards[found].planName, sizeof(standards[found].planName));
                    } else if (mod == 2) {
                        printf("请输入分段数: ");
                        int n = get_int("");
                        
                        if (n < 1 || n > MAX_SEGMENTS) {
                            printf("段数必须在1-%d之间！\n", MAX_SEGMENTS);
                            break;
                        }
                        
                        standards[found].segmentCount = n;
                        
                        for (int i = 0; i < n; i++) {
                            printf("\n第%d段：\n", i + 1);
                            
                            if (i == 0) {
                                printf("起始小时（必须是0）: ");
                                double start = 0;
                                scanf("%lf", &start);
                                getchar();
                                if (start != 0) {
                                    printf("第一段起始必须是0！\n");
                                    i--;
                                    continue;
                                }
                                standards[found].segments[i].startHour = start;
                            } else {
                                standards[found].segments[i].startHour = standards[found].segments[i-1].endHour;
                                printf("起始小时（自动继承上段终止）: %.1f\n", standards[found].segments[i].startHour);
                            }
                            
                            if (i == n - 1) {
                                printf("终止小时（输入-1表示无限制）: ");
                                double end = -1;
                                scanf("%lf", &end);
                                getchar();
                                if (end != -1) {
                                    printf("最后一段终止必须是-1！\n");
                                    i--;
                                    continue;
                                }
                                standards[found].segments[i].endHour = end;
                            } else {
                                printf("终止小时（不能是-1）: ");
                                double end = 0;
                                scanf("%lf", &end);
                                getchar();
                                if (end == -1) {
                                    printf("非最后一段不能输入-1！\n");
                                    i--;
                                    continue;
                                }
                                if (end <= standards[found].segments[i].startHour) {
                                    printf("终止小时必须大于起始小时！\n");
                                    i--;
                                    continue;
                                }
                                standards[found].segments[i].endHour = end;
                            }
                            
                            printf("该段单价（元/小时）: ");
                            double price = 0;
                            scanf("%lf", &price);
                            getchar();
                            if (price <= 0) {
                                printf("价格必须大于0！\n");
                                i--;
                                continue;
                            }
                            standards[found].segments[i].pricePerHour = price;
                        }
                    }
                    
                    save_billing(standards, standard_count);
                    printf("修改成功！\n");
                }
                break;
                
            case 4:
                printf("\n删除套餐\n");
                get_string("请输入要删除的套餐ID: ", planId, sizeof(planId));
                {
                    int found = -1;
                    for (int i = 0; i < standard_count; i++) {
                        if (standards[i].nDel == 0 && strcmp(standards[i].planId, planId) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found == -1) {
                        printf("套餐不存在！\n");
                        break;
                    }
                    
                    int activeCount = 0;
                    for (int i = 0; i < standard_count; i++) {
                        if (standards[i].nDel == 0) activeCount++;
                    }
                    if (activeCount <= 1) {
                        printf("不能删除最后一个套餐！\n");
                        break;
                    }
                    
                    standards[found].nDel = 1;
                    save_billing(standards, standard_count);
                    printf("删除成功！\n");
                }
                break;
                
            case 5:
                printf("\n设置默认套餐\n");
                get_string("请输入默认套餐ID: ", planId, sizeof(planId));
                {
                    int found = -1;
                    for (int i = 0; i < standard_count; i++) {
                        if (standards[i].nDel == 0 && strcmp(standards[i].planId, planId) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found == -1) {
                        printf("套餐不存在！\n");
                        break;
                    }
                    
                    BillingStandard temp = standards[0];
                    standards[0] = standards[found];
                    standards[found] = temp;
                    save_billing(standards, standard_count);
                    printf("默认套餐已设置为：%s\n", standards[0].planName);
                }
                break;
        }
    }
}
