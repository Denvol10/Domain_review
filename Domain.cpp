

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <functional>

using namespace std;
using namespace std::literals;

class Domain {
public:
    Domain() = default;
    
    Domain(std::string domain)
        : domain_(string(domain.rbegin(), domain.rend())) {
    }

    bool operator==(const Domain& right) const {
        return this->domain_ == right.domain_;
    }

    bool operator<(const Domain& right) const {
        return this->domain_ < right.domain_;
    }

    // метод IsSubdomain, принимающий другой домен и возвращающий true, если this его поддомен
    bool IsSubdomain(const Domain& sub_domain) const {
        size_t n = sub_domain.domain_.find(this->domain_ + "."s);
        return this->domain_ == sub_domain.domain_ || n == 0;
    }

    const std::string& GetAddresDomain() const {
        return domain_;
    }

private:
    std::string domain_;
};


class DomainChecker {
public:
    // конструктор должен принимать список запрещённых доменов через пару итераторов
    template<typename Iterator>
    DomainChecker(Iterator begin, Iterator end)
        : forbidden_domains_( std::vector<Domain>(end - begin) ) {
        
        for(int i = 0 ; i<end-begin ; ++i) {
            forbidden_domains_[i] = move(*(begin + i));
        }

        sort(forbidden_domains_.begin(), forbidden_domains_.end()
            , [](const Domain& d1, const Domain& d2){
                return d1 < d2;
            }
        );

        auto iter_end = unique(forbidden_domains_.begin(), forbidden_domains_.end()
            , [](const Domain& d1, const Domain& d2){
                return d1.IsSubdomain(d2) || d2.IsSubdomain(d1);
            }
        );

        forbidden_domains_.resize(iter_end - forbidden_domains_.begin());
    }

    // метод IsForbidden, возвращающий true, если домен запрещён
    bool IsForbidden(const Domain& test_domain) {
        // Ищем самый первый элемент, который > test_domain
        auto iter = upper_bound(forbidden_domains_.begin(), forbidden_domains_.end(), test_domain);
        if (iter == forbidden_domains_.begin()) {
            return false;
        }
        --iter;                                 // Кандидат — предыдущий элемент (наибольший <= test_domain)
        return iter->IsSubdomain(test_domain);  // Проверяем, является ли кандидат префиксом test_domain значит, test_domain — поддомен
    }

private:
    std::vector<Domain> forbidden_domains_;
};


// ReadDomains, читающую заданное количество доменов из стандартного входа
std::vector<Domain> ReadDomains(istream& s_in, size_t n) {
    std::vector<Domain> domains;
    domains.reserve(n);

    string line;
    for(size_t i = 0 ; i<n ; ++i) {     // Считываем данные из потока построчно в переменную text.
        getline(s_in, line);
        domains.emplace_back(line);
    }
    return domains;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;
    return num;
}

int main() {
    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}

