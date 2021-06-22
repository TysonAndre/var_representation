<?php
function dump($value): void {
    echo var_representation($value), "\n";
    $output = var_representation($value, VAR_REPRESENTATION_SINGLE_LINE);
    echo "oneline: $output\n";
}
