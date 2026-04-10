#include "service.h"
#include "adminservice.h"
#include "accountservice.h"
#include "loginout.h"
#include "billingstandard.h"
#include "financeservice.h"
#include "logsearch.h"
#include <stdio.h>

void service_dispatch(int option) {
    switch (option) {
        case 1:
            admin_service();
            break;
        case 2:
            account_service();
            break;
        case 3:
            loginout_service();
            break;
        case 4:
            billing_standard_service();
            break;
        case 5:
            finance_service();
            break;
        case 6:
            log_search_service();
            break;
        default:
            printf("无效的服务选项。\n");
            break;
    }
}
