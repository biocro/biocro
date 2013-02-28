BioCro fitting, parameterization, and testing 
========================================================



```r
install_bitbucket(repo = "pecandb", username = "dlebauer")
require(pecandb)
data(bety, package = "pecandb")
salix.yields <- get.yield.DT(list = bety)[genus == "Salix"]
save(salix.yields, file = "inst/extdata/salix.RData")
```



```
## Loading required package: data.table
```

```
## Loading required package: lubridate
```

```
## Attaching package: 'lubridate'
```

```
## The following object(s) are masked from 'package:data.table':
## 
## hour, mday, month, quarter, wday, week, yday, year
```

```
## Loading required package: ggplot2
```

```
## Warning: Removed 1043 rows containing missing values (geom_path).
```

```
## Warning: Removed 93 rows containing missing values (geom_point).
```

```
## Warning: Removed 93 rows containing missing values (geom_text).
```

![plot of chunk unnamed-chunk-2](figure/unnamed-chunk-21.png) 

```
## Warning: Removed 869 rows containing missing values (geom_path).
```

```
## Warning: Removed 63 rows containing missing values (geom_point).
```

```
## Warning: Removed 63 rows containing missing values (geom_text).
```

![plot of chunk unnamed-chunk-2](figure/unnamed-chunk-22.png) 

```
## 185 parsed with %Y-%m-%d
```

```
## 185 parsed with %Y-%m-%d
```

![plot of chunk unnamed-chunk-2](figure/unnamed-chunk-23.png) 



```r
trait.summary <- salix.traits[sort(n), list(n = length(site_id)), by = list(trait)]
trait.summary2 <- trait.summary[with(trait.summary, rank(n + rank(trait)/1000)), 
    ]
ggplot(data = trait.summary, aes(x = trait, y = n, order = n + rank(trait)/100)) + 
    geom_point() + geom_linerange(aes(ymin = 0, ymax = n))  #+ coord_flip() + theme_bw()
```

![plot of chunk unnamed-chunk-3](figure/unnamed-chunk-3.png) 

