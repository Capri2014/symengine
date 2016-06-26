#include <symengine/series_visitor.h>

#ifdef HAVE_SYMENGINE_FLINT
namespace SymEngine
{

URatPSeriesFlint::URatPSeriesFlint(fp_t p, const std::string varname,
                                   const unsigned degree)
    : SeriesBase(std::move(p), varname, degree)
{
}
RCP<const URatPSeriesFlint> URatPSeriesFlint::series(const RCP<const Basic> &t,
                                                     const std::string &x,
                                                     unsigned int prec)
{
    fp_t p("2  0 1");
    SeriesVisitor<fp_t, fmpq_wrapper, URatPSeriesFlint> visitor(p, x, prec);
    return visitor.series(t);
}

std::size_t URatPSeriesFlint::__hash__() const
{
    std::hash<std::string> str_hash;
    std::size_t seed = URATPSERIESFLINT;
    hash_combine(seed, var_);
    hash_combine(seed, degree_);
    hash_combine(seed, str_hash(p_.to_string()));
    return seed;
}

RCP<const Basic> URatPSeriesFlint::as_basic() const
{
    RCP<const Symbol> x = symbol(var_);
    RCP<const Number> zcoef;
    umap_basic_num dict_;
    mpq_t gc;
    mpq_init(gc);
    for (int n = 0; n < degree_; n++) {
        const fmpq_wrapper fc(p_.get_coeff(n));
        if (not fc.is_zero()) {
            fmpq_get_mpq(gc, fc.get_fmpq_t());
            RCP<const Number> basic = Rational::from_mpq(rational_class(gc));
            auto term = SymEngine::mul(SymEngine::pow(x, SymEngine::integer(n)),
                                       basic);
            if (n == 0)
                zcoef = basic;
            Add::coef_dict_add_term(outArg(basic), dict_, one, term);
        } else if (n == 0)
            zcoef = integer(0);
    }
    mpq_clear(gc);
    return std::move(Add::from_dict(zcoef, std::move(dict_)));
}

umap_int_basic URatPSeriesFlint::as_dict() const
{
    umap_int_basic map;
    mpq_t gc;
    mpq_init(gc);
    for (int n = 0; n < degree_; n++) {
        const fmpq_wrapper fc(p_.get_coeff(n));
        if (not fc.is_zero()) {
            fmpq_get_mpq(gc, fc.get_fmpq_t());
            RCP<const Number> basic = Rational::from_mpq(rational_class(gc));
            map[n] = basic;
        }
    }
    mpq_clear(gc);
    return map;
}

RCP<const Basic> URatPSeriesFlint::get_coeff(int n) const
{
    const fmpq_wrapper fc(p_.get_coeff(n));
    mpq_t gc;
    mpq_init(gc);
    fmpq_get_mpq(gc, fc.get_fmpq_t());
    rational_class r(gc);
    mpq_clear(gc);
    return Rational::from_mpq(std::move(r));
}

int URatPSeriesFlint::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<URatPSeriesFlint>(o))
    const URatPSeriesFlint &s = static_cast<const URatPSeriesFlint &>(o);
    if (var_ != s.var_)
        return (var_ < s.var_) ? -1 : 1;
    if (degree_ != s.degree_)
        return (degree_ < s.degree_) ? -1 : 1;
    if (p_ == s.p_)
        return 0;
    return (p_ < s.p_) ? -1 : 1;
}

fp_t URatPSeriesFlint::var(const std::string &s)
{
    fp_t r("2  0 1");
    return r;
}

fp_t URatPSeriesFlint::convert(const rational_class &x)
{
    return fp_t(get_mpq_t(x));
}

fp_t URatPSeriesFlint::convert(const Integer &x)
{
    return fmpq_wrapper(x.as_mpz());
}

fp_t URatPSeriesFlint::convert(const Rational &x)
{
    return fmpq_wrapper(x.as_rational_class());
}

fp_t URatPSeriesFlint::convert(const Basic &x)
{
    throw std::runtime_error("SeriesFlint::convert not Implemented");
}

fp_t URatPSeriesFlint::pow(const fp_t &s, int n, unsigned prec)
{
    if (n > 0)
        return fp_t(s.pow(unsigned(n)));
    else if (n < 0)
        return fp_t(s.inv_series(prec).pow(unsigned(-n)));
    return fp_t("1 1");
}

unsigned URatPSeriesFlint::ldegree(const fp_t &s)
{
    long i = 0;
    while (i <= s.degree())
        if (not s.get_coeff(i++).is_zero())
            return i - 1;
    return 0;
}

fmpq_wrapper URatPSeriesFlint::root(fmpq_wrapper &c, unsigned n)
{
    fmpq_wrapper cl_rat = c, cl_root;
    cl_rat.canonicalise();
    cl_root.get_num() = cl_rat.get_num().root(n);
    if (cl_rat.get_den() == 1)
        cl_root.get_den() = 1;
    else
        cl_root.get_den() = cl_rat.get_den().root(n);
    return cl_root;
}

fp_t URatPSeriesFlint::diff(const fp_t &s, const fp_t &var)
{
    return s.derivative();
}

fp_t URatPSeriesFlint::integrate(const fp_t &s, const fp_t &var)
{
    return s.integral();
}

fp_t URatPSeriesFlint::subs(const fp_t &s, const fp_t &var, const fp_t &r,
                            unsigned prec)
{
    return s.subs(r, prec);
}
}
#endif // HAVE_SYMENGINE_FLINT
