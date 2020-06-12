void swapSx::tradelog( const name buyer,
                       const asset quantity,
                       const asset rate,
                       const asset fee,
                       const double trade_price
) {
    require_auth( get_self() );
    if ( is_account("log.sx"_n) ) require_recipient("log.sx"_n);
}