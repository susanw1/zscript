pub mod token_buffer;
pub mod tokenizer;
pub mod z_chars;


fn main() {
    let mut a: Apple = Apple { size: 2 };
    println!("{}", a.size);
    a.grow();
    println!("{}", a.size);
    a.throw();
    Fruit::eat(&mut a);
    println!("{}", a.size);
    let result: u64 = factorial(10);
    println!("{}", result);
}

