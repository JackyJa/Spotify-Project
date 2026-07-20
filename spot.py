
import sqlite3
from typing import List

import torch
from transformers import AutoTokenizer, AutoModelForCausalLM, pipeline

from langchain_core.documents import Document
from langchain_core.prompts import PromptTemplate
from langchain_text_splitters import RecursiveCharacterTextSplitter
from langchain_community.vectorstores import FAISS
from langchain_community.embeddings import HuggingFaceEmbeddings
from langchain_community.llms import HuggingFacePipeline
from transformers import BitsAndBytesConfig




torch.cuda.empty_cache()
DB_PATH = "A:/Qt_Ptojects/Project/build/Desktop_Qt_6_11_1_MSVC2022_64bit_Debug/spotify.db"
MODEL_NAME = "A:\Qwen_2.5_3B"
EMBEDDING_MODEL = "B:\MiniLM"





import sqlite3
from typing import List
from langchain_core.documents import Document

def load_sqlite_as_documents(db_path: str) -> List[Document]:
    conn = sqlite3.connect(db_path)
    conn.row_factory = sqlite3.Row
    cur = conn.cursor()
    docs = []

  
    cur.execute("""
        SELECT 
            s.*, 
            a.fullName AS artist_fullName, a.userName AS artist_userName,
            al.name AS album_name, al.coverPath AS album_cover,
            (SELECT COUNT(*) FROM likes l WHERE l.songId = s.id) AS total_likes
        FROM songs s
        LEFT JOIN accounts a ON s.artistId = a.id
        LEFT JOIN albums al ON s.albumId = al.id
    """)
    for row in cur.fetchall():
        content = (
            f"Entity: Song\n"
            f"ID: {row['id']}\n"
            f"Name: {row['name']}\n"
            f"Release Year: {row['releaseYear']}\n"
            f"Genre: {row['genre']}\n"
            f"Audio File Path: {row['audioFilePath']}\n"
            f"Cover Path: {row['coverPath']}\n"
            f"Artist: {row['artist_fullName']} (@{row['artist_userName']})\n"
            f"Album: {row['album_name']}\n"
            f"Total Likes: {row['total_likes']}"
        )
        docs.append(Document(page_content=content, metadata={"source": "songs", "id": row['id']}))

  
    cur.execute("""
        SELECT al.*, a.fullName AS artist_fullName 
        FROM albums al
        LEFT JOIN accounts a ON al.artistId = a.id
    """)
    for row in cur.fetchall():
        cur2 = conn.cursor()
        cur2.execute("SELECT name FROM songs WHERE albumId = ?", (row['id'],))
        songs = [r['name'] for r in cur2.fetchall()]

        content = (
            f"Entity: Album\n"
            f"ID: {row['id']}\n"
            f"Name: {row['name']}\n"
            f"Cover Path: {row['coverPath']}\n"
            f"Artist: {row['artist_fullName']}\n"
            f"Tracklist: {', '.join(songs) if songs else 'No songs'}"
        )
        docs.append(Document(page_content=content, metadata={"source": "albums", "id": row['id']}))

    
    cur.execute("SELECT * FROM accounts")
    for row in cur.fetchall():
        content = (
            f"Entity: Account/Artist\n"
            f"ID: {row['id']}\n"
            f"Full Name: {row['fullName']}\n"
            f"Username: {row['userName']}\n"
            f"Biography: {row['biography']}\n"
            f"Role: {row['role']}"
        )
        docs.append(Document(page_content=content, metadata={"source": "accounts", "id": row['id']}))

    
    cur.execute("""
        SELECT p.*, a.fullName AS creator_name 
        FROM playlists p
        LEFT JOIN accounts a ON p.listenerId = a.id
    """)
    for row in cur.fetchall():
        cur2 = conn.cursor()
        cur2.execute("""
            SELECT s.name FROM playlist_songs ps 
            JOIN songs s ON ps.songId = s.id 
            WHERE ps.playlistId = ?
        """, (row['id'],))
        songs = [r['name'] for r in cur2.fetchall()]

        content = (
            f"Entity: Playlist\n"
            f"ID: {row['id']}\n"
            f"Name: {row['name']}\n"
            f"Created By (Listener): {row['creator_name']}\n"
            f"Songs: {', '.join(songs) if songs else 'Empty'}"
        )
        docs.append(Document(page_content=content, metadata={"source": "playlists", "id": row['id']}))

    conn.close()
    return docs





def build_vectorstore(documents: List[Document]):
    splitter = RecursiveCharacterTextSplitter(
        chunk_size=500,
        chunk_overlap=50
    )

    split_docs = splitter.split_documents(documents)
    print("Number of chunks:", len(split_docs))

    embeddings = HuggingFaceEmbeddings(
        model_name=EMBEDDING_MODEL
    )

    vectorstore = FAISS.from_documents(split_docs, embeddings)
    return vectorstore





def build_llm():
    tokenizer = AutoTokenizer.from_pretrained(MODEL_NAME)
    
    use_cuda = torch.cuda.is_available()
    
    model = AutoModelForCausalLM.from_pretrained(
        MODEL_NAME,
        torch_dtype=torch.float16 if use_cuda else torch.float32,
        device_map="auto",
        low_cpu_mem_usage=True
    )

    text_gen_pipeline = pipeline(
        "text-generation",
        model=model,
        tokenizer=tokenizer,
        max_new_tokens=50,
        temperature=0.0,
        do_sample=False,
        repetition_penalty=1.1,
        return_full_text=False
    )

    llm = HuggingFacePipeline(pipeline=text_gen_pipeline)
    return llm





PROMPT_TEMPLATE = """<|im_start|>system
You are a database-only question answering assistant.

Rules:
1. Answer ONLY from the provided Context.
2. Do NOT use prior knowledge, world knowledge, memory, or assumptions.
3. If the exact answer is not explicitly موجود in the Context, reply exactly with:
اطلاعات کافی در دیتابیس پیدا نکردم.
4. If the question is unrelated to the Context or asks about your identity, opinions, reasoning, or anything not stored in the database, reply exactly with:
اطلاعات کافی در دیتابیس پیدا نکردم.
5. Output only the final answer value, with no explanation, no extra words, no formatting.

<|im_end|>
<|im_start|>user
Context:
{context}

Question: {question}
<|im_end|>
<|im_start|>assistant
"""




prompt = PromptTemplate(
    template=PROMPT_TEMPLATE,
    input_variables=["context", "question"]
)





def ask_rag(question: str, vectorstore, llm, k: int = 5):
    docs = vectorstore.similarity_search(question, k=k)
    context = "\n\n".join(doc.page_content for doc in docs)

    final_prompt = prompt.format(context=context, question=question)

    
    raw_answer = llm.invoke(final_prompt)

   
    answer = raw_answer.split("<|im_end|>")[0].strip()
    answer = answer.split("\n")[0].strip() 

    return {"answer": answer, "documents": docs}





documents = load_sqlite_as_documents(DB_PATH)
print("Loaded documents:", len(documents))

vectorstore = build_vectorstore(documents)
print("Vector store is ready.")

llm = build_llm()
print("LLM is ready.")


from fastapi import FastAPI
from pydantic import BaseModel
import uvicorn

app = FastAPI()

class QuestionRequest(BaseModel):
    question: str

@app.on_event("startup")
def startup_event():
    global vectorstore, llm
    print("Loading documents and models...")
    documents = load_sqlite_as_documents(DB_PATH)
    vectorstore = build_vectorstore(documents)
    llm = build_llm()
    print("RAG system is ready!")

@app.post("/ask")
def ask_question(request: QuestionRequest):
    try:
        result = ask_rag(request.question, vectorstore, llm, k=5)
        return {"answer": result["answer"]}
    except Exception as e:
        return {"answer": f"Error in RAG: {str(e)}"}

if __name__ == "__main__":
    
    uvicorn.run(app, host="127.0.0.1", port=8000)



